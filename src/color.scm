(import (chicken io)
        (chicken irregex)
        (chicken pathname)
        (chicken process-context)
        (chicken port)
        (chicken string)
        (format)
        (matchable)
        (mathh))

;; ---
;; general functions
;; ---

(define (error-message status . args)
  (with-output-to-port (current-output-port)
    (lambda () (apply print args)))
  (exit status))

(define (usage)
  (error-message 1 "usage : " (pathname-file (program-name)) " [-rgbcmykhsl <value>]"))

(define (convert str)
  (let ((tmp (string->number str)))
    (if tmp tmp
        (error-message 1 "error : '" str "' isn't a valid value"))))

(define (fp->integer fp)
  (inexact->exact (round fp)))

(define (edit a b)
  (map (cut + <> <>) a b))

(define (generate-replacements lst rgb cmyk hsl)
  (map
    (lambda (str)
      (let* ((i (str->rgb str))
             (i (edit   i rgb))
             (i (fix-rgb    i))
             (i (rgb->cmyk  i))
             (i (edit  i cmyk))
             (i (fix-cmyk   i))
             (i (cmyk->rgb  i))
             (i (rgb->hsl   i))
             (i (edit   i hsl))
             (i (fix-hsl    i))
             (i (hsl->rgb   i))
             (i (rgb->str   i)))
        (cons str i)))
    lst))

(define (edit-input rgb cmyk hsl)
  (map
    (lambda (str)
      (let ((alist (generate-replacements (irregex-extract "#[0-9A-f]{6}" str) rgb cmyk hsl)))
        (print (string-translate* str alist))))
    (read-lines)))

;; ---
;; fix values
;; ---

(define (fix item a b)
  (cond ((< item a) a)
        ((> item b) b)
        (else item)))

(define (fix-rgb rgb)
  (map (cut fix <> 0.0 255.0) rgb))

(define (fix-cmyk cmyk)
  (map (cut fix <> 0.0 1.0) cmyk))

(define (fix-hsl hsl)
  (match hsl
    ((h s l) (cons (fix h 0.0 360.0) (map (cut fix <> 0.0 1.0) (list s l))))))

;; ---
;; rgb & str
;; ---

(define (str->rgb str)
  (match (string-split str "#")
    ((str) (map (cut string->number <> 16) (string-chop str 2)))))

(define (rgb->str rgb)
  (apply (cut format "#~2,'0X~2,'0X~2,'0X" <> <> <>) rgb))

;; ---
;; rgb & cmyk
;; ---

;; https://www.rapidtables.com/convert/color/rgb-to-cmyk.html
(define (rgb->cmyk rgb)
  (let ((rgb (map (cut / <> 255.0) rgb)))
    (let ((k (- 1.0 (apply max rgb))))
      (append (map
                (lambda (i)
                  ;; prevent division by 0
                  (if (= k 1.0) 0.0
                      (/ (- 1.0 i k) (- 1.0 k))))
                rgb)
              (list k)))))

;; https://www.rapidtables.com/convert/color/cmyk-to-rgb.html
(define (cmyk->rgb cmyk)
  (match cmyk
    ((c m y k)
     (map
       (lambda (i)
         (fp->integer (* 255.0 (- 1.0 i) (- 1.0 k))))
       (list c m y)))))

;; ---
;; rgb & hsl
;; ---

(define (get-hue rgb maximum delta)
  (match rgb
    ((r g b)
     (let ((tmp (cond ((= delta 0.0) 0.0)
                      ((= maximum g) (+ 2.0 (/ (- b r) delta)))
                      ((= maximum b) (+ 4.0 (/ (- r g) delta)))
                      ((= maximum r) (fpmod (/ (- g b) delta) 6.0)))))
       (fpmod (+ 360.0 (* tmp 60.0)) 360.0)))))

(define (get-saturation lightness delta)
  (if (= delta 0.0) 0.0
      (/ delta (- 1.0 (abs (+ lightness lightness -1))))))

(define (get-lightness maximum minimum)
  (/ (+ maximum minimum) 2.0))

;; https://www.rapidtables.com/convert/color/rgb-to-hsl.html
(define (rgb->hsl rgb)
  (let ((rgb (map (cut / <> 255.0) rgb)))
    (match rgb
      ((r g b)
       (let* ((maximum (apply max rgb))
              (minimum (apply min rgb))
              (delta (- maximum minimum)))
         (let ((lightness (get-lightness maximum minimum)))
           (list (get-hue rgb maximum delta) (get-saturation lightness delta) lightness)))))))

(define (get-c l s)
  (* (- 1.0 (abs (+ l l -1.0))) s))

(define (get-x c h)
  (* (- 1.0 (abs (- (fpmod (/ h 60.0) 2.0) 1.0))) c))

(define (get-m c l)
  (- l (/ c 2.0)))

;; https://www.rapidtables.com/convert/color/hsl-to-rgb.html
(define (hsl->rgb hsl)
  (match hsl
    ((h s l)
     (let* ((c (get-c l s))
            (x (get-x c h))
            (m (get-m c l)))
       (map
         (lambda (i)
           (fp->integer (* 255.0 (+ i m))))
         (cond ((< h  60.0) (list   c   x 0.0))
               ((< h 120.0) (list   x   c 0.0))
               ((< h 180.0) (list 0.0   c   x))
               ((< h 240.0) (list 0.0   x   c))
               ((< h 300.0) (list   x 0.0   c))
               ((< h 360.0) (list   c 0.0   x))))))))

;; ---
;; argument parsing
;; ---

(let ((r 0) (g 0) (b 0) (c 0) (m 0) (y 0) (k 0) (h 0) (s 0) (l 0))
  (let main ((args (command-line-arguments)))
    (match args
      ((arg value . tail)
       (let ((value (convert value)))
         (case (string->symbol arg)
           ((-r) (set! r (+ r value)))
           ((-g) (set! g (+ g value)))
           ((-b) (set! b (+ b value)))
           ((-c) (set! c (+ c value)))
           ((-m) (set! m (+ m value)))
           ((-y) (set! y (+ y value)))
           ((-k) (set! k (+ k value)))
           ((-h) (set! h (+ h value)))
           ((-s) (set! s (+ s value)))
           ((-l) (set! l (+ l value)))
           (else (usage))))
       (main tail))
      (()
       (edit-input
         (list   r g b)
         (list c m y k)
         (list   h s l)))
      ((_) (usage)))))
