;; -*-scheme-*-

;; Alan Horkan 2004.  Copyright.  
;; I'll fix it and license it differntly later if anyone cares to ask

(define (tiny-fu-guide-new-percent image
				     drawable
				     direction
				     position)
  (let* ((width (car (gimp-drawable-width drawable)))
	 (height (car (gimp-drawable-height drawable))))
    (gimp-image-undo-group-start image)

    (if (= direction 0)
	(set! position (/ (* height position) 100))
	(set! position (/ (* width position) 100)))

    (if (= direction 0) 
	;; convert position to pixel 
	(if (< position height) (gimp-image-add-hguide image position))
	(if (< position width) (gimp-image-add-vguide image position)))

    (gimp-image-undo-group-end image)
    (gimp-displays-flush)))
    
(tiny-fu-register "tiny-fu-guide-new-percent"
		    _"<Image>/Image/Guides/New Guide (by _Percent)..." 
		    "Add a single Line Guide with the specified postion. Position specified as a percent of the image size."
		    "Alan Horkan"
		    "Alan Horkan, 2004"
		    "April 2004"
		    ""
		    SF-IMAGE      "Input Image"      0 
		    SF-DRAWABLE   "Input Drawable"   0
		    SF-OPTION     _"Direction"       '(_"Horizontal" 
						       _"Vertical")
		    SF-ADJUSTMENT _"Position (in %)" '(50 0 100 1 10 0 1))