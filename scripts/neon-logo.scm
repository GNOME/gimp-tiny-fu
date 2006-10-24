;  NEON
;  Create a text effect that simulates neon lighting

(define (apply-neon-logo-effect img
                                tube-layer
                                size
                                bg-color
                                glow-color
                                shadow)

  (define (set-pt a index x y)
	(begin
	 (aset a (* index 2) x)
	 (aset a (+ (* index 2) 1) y)))

  (define (neon-spline1)
	(let* ((a (cons-array 6 'byte)))
	  (set-pt a 0 0 0)
	  (set-pt a 1 127 145)
	  (set-pt a 2 255 255)
	  a))

  (define (neon-spline2)
	(let* ((a (cons-array 6 'byte)))
	  (set-pt a 0 0 0)
	  (set-pt a 1 110 150)
	  (set-pt a 2 255 255)
	  a))

  (define (neon-spline3)
	(let* ((a (cons-array 6 'byte)))
	  (set-pt a 0 0 0)
	  (set-pt a 1 100 185)
	  (set-pt a 2 255 255)
	  a))

  (define (neon-spline4)
	(let* ((a (cons-array 8 'byte)))
	  (set-pt a 0 0 0)
	  (set-pt a 1 64 64)
	  (set-pt a 2 127 192)
	  (set-pt a 3 255 255)
	  a))

  (define (find-hue-offset color)
	(let* (
		  (R (car color))
		  (G (cadr color))
		  (B (caddr color))
		  (max-val (max R G B))
		  (min-val (min R G B))
		  (delta (- max-val min-val))
		  (hue 0)
		  )
	  (if (= delta 0)
		  0
		  (begin
			(cond
			  ((= max-val R) (set! hue (/ (- G B) (* 1.0 delta))))
			  ((= max-val G) (set! hue (+ 2 (/ (- B R) (* 1.0 delta)))))
			  ((= max-val B) (set! hue (+ 4 (/ (- R G) (* 1.0 delta)))))
			)
			(set! hue (* hue 60))
			(if (< hue 0) (set! hue (+ hue 360)))
			(if (> hue 360) (set! hue (- hue 360)))
			(if (> hue 180) (set! hue (- hue 360)))
			hue
		  )
	  )
	)
  )

  (let* (
        (tube-hue (find-hue-offset glow-color))
        (shrink (/ size 14))
        (grow (/ size 40))
        (feather (/ size 5))
        (feather1 (/ size 25))
        (feather2 (/ size 12))
        (inc-shrink (/ size 100))
        (shadow-shrink (/ size 40))
        (shadow-feather (/ size 20))
        (shadow-offx (/ size 10))
        (shadow-offy (/ size 10))
        (width (car (gimp-drawable-width tube-layer)))
        (height (car (gimp-drawable-height tube-layer)))
        (glow-layer (car (gimp-layer-new img width height RGBA-IMAGE "Neon Glow" 100 NORMAL-MODE)))
        (bg-layer (car (gimp-layer-new img width height RGB-IMAGE "Background" 100 NORMAL-MODE)))
        (shadow-layer (if (= shadow TRUE)
                          (car (gimp-layer-new img width height RGBA-IMAGE "Shadow" 100 NORMAL-MODE))
                          0))
        (selection 0)
        )

    (gimp-context-push)

    (script-fu-util-image-resize-from-layer img tube-layer)
    (gimp-image-add-layer img bg-layer 1)
    (if (not (= shadow 0))
        (begin
          (gimp-image-add-layer img shadow-layer 1)
          (gimp-edit-clear shadow-layer)))
    (gimp-image-add-layer img glow-layer 1)

    (gimp-context-set-background '(0 0 0))
    (gimp-selection-layer-alpha tube-layer)
    (set! selection (car (gimp-selection-save img)))
    (gimp-selection-none img)

    (gimp-edit-clear glow-layer)
    (gimp-edit-clear tube-layer)

    (gimp-context-set-background bg-color)
    (gimp-edit-fill bg-layer BACKGROUND-FILL)

    (gimp-selection-load selection)
    (gimp-context-set-background '(255 255 255))
    (gimp-edit-fill tube-layer BACKGROUND-FILL)
    (gimp-selection-shrink img shrink)
    (gimp-context-set-background '(0 0 0))
    (gimp-edit-fill selection BACKGROUND-FILL)
    (gimp-edit-clear tube-layer)

    (gimp-selection-none img)
    (if (not (= feather1 0)) (plug-in-gauss-rle 1 img tube-layer feather1 TRUE TRUE))
    (gimp-selection-load selection)
    (if (not (= feather2 0)) (plug-in-gauss-rle 1 img tube-layer feather2 TRUE TRUE))

    (gimp-selection-feather img inc-shrink)
    (gimp-selection-shrink img inc-shrink)
    (gimp-curves-spline tube-layer 4 6 (neon-spline1))

    (gimp-selection-load selection)
    (gimp-selection-feather img inc-shrink)
    (gimp-selection-shrink img (* inc-shrink 2))
    (gimp-curves-spline tube-layer 4 6 (neon-spline2))

    (gimp-selection-load selection)
    (gimp-selection-feather img inc-shrink)
    (gimp-selection-shrink img (* inc-shrink 3))
    (gimp-curves-spline tube-layer 4 6 (neon-spline3))

    (gimp-layer-set-lock-alpha tube-layer 1)
    (gimp-selection-layer-alpha tube-layer)
    (gimp-selection-invert img)
    (gimp-context-set-background glow-color)
    (gimp-edit-fill tube-layer BACKGROUND-FILL)

    (gimp-selection-none img)
    (gimp-layer-set-lock-alpha tube-layer 0)
    (gimp-curves-spline tube-layer 4 8 (neon-spline4))

    (gimp-selection-load selection)
    (gimp-selection-grow img grow)
    (gimp-selection-invert img)
    (gimp-edit-clear tube-layer)
    (gimp-selection-invert img)

    (gimp-selection-feather img feather)
    (gimp-edit-fill glow-layer BACKGROUND-FILL)

    (if (not (= shadow 0))
        (begin
          (gimp-selection-load selection)
          (gimp-selection-grow img grow)
          (gimp-selection-shrink img shadow-shrink)
          (gimp-selection-feather img shadow-feather)
          (gimp-selection-translate img shadow-offx shadow-offy)
          (gimp-context-set-background '(0 0 0))
          (gimp-edit-fill shadow-layer BACKGROUND-FILL)))
    (gimp-selection-none img)

    (gimp-drawable-set-name tube-layer "Neon Tubes")
    (gimp-image-remove-channel img selection)

    (gimp-context-pop)
  )
)

(define (script-fu-neon-logo-alpha img
                                   tube-layer
                                   size
                                   bg-color
                                   glow-color
                                   shadow)
  (begin
    (gimp-image-undo-group-start img)
    (apply-neon-logo-effect img tube-layer size bg-color glow-color shadow)
    (gimp-image-undo-group-end img)
    (gimp-displays-flush)
  )
)

(script-fu-register "script-fu-neon-logo-alpha"
  _"N_eon..."
  _"Convert the selected region (or alpha) into a neon-sign like object"
  "Spencer Kimball"
  "Spencer Kimball"
  "1997"
  "RGBA"
  SF-IMAGE      "Image"                     0
  SF-DRAWABLE   "Drawable"                  0
  SF-ADJUSTMENT _"Effect size (pixels * 5)" '(150 2 1000 1 10 0 1)
  SF-COLOR      _"Background color"         "black"
  SF-COLOR      _"Glow color"               '(38 211 255)
  SF-TOGGLE     _"Create shadow"            FALSE
)

(script-fu-menu-register "script-fu-neon-logo-alpha"
                         "<Image>/Filters/Alpha to Logo")

(define (script-fu-neon-logo text
                             size
                             font
                             bg-color
                             glow-color
                             shadow)
  (let* (
        (img (car (gimp-image-new 256 256 RGB)))
        (border (/ size 4))
        (tube-layer (car (gimp-text-fontname img -1 0 0 text border TRUE size PIXELS font)))
        )
    (gimp-image-undo-disable img)
    (apply-neon-logo-effect img tube-layer size bg-color glow-color shadow)
    (gimp-image-undo-enable img)
    (gimp-display-new img)
  )
)

(script-fu-register "script-fu-neon-logo"
  _"N_eon..."
  _"Create a logo in the style of a neon sign"
  "Spencer Kimball"
  "Spencer Kimball"
  "1997"
  ""
  SF-STRING     _"Text"               "NEON"
  SF-ADJUSTMENT _"Font size (pixels)" '(150 2 1000 1 10 0 1)
  SF-FONT       _"Font"               "Blippo"
  SF-COLOR      _"Background color"   "black"
  SF-COLOR      _"Glow color"         '(38 211 255)
  SF-TOGGLE     _"Create shadow"      FALSE
)

(script-fu-menu-register "script-fu-neon-logo"
                         "<Toolbox>/Xtns/Logos")
