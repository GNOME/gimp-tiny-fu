; The GIMP -- an image manipulation program
; Copyright (C) 1995 Spencer Kimball and Peter Mattis
; 
; Circuit board effect
; Copyright (c) 1997 Adrian Likins
; aklikins@eos.ncsu.ed
;
;  Genrates what looks a little like the back of an old circuit board.
;  Looks even better when gradmapped with a suitable gradient.
;
; This script doesnt handle or color combos well. ie, black/black 
;  doesnt work..
;  The effect seems to work best on odd shaped selections because of some
; limitations in the maze codes selection handling ablity
;
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


(define (tiny-fu-circuit image
                         drawable
                         mask-size
                         seed
                         remove-bg
                         keep-selection
                         separate-layer)
  (let* (
        (type (car (gimp-drawable-type-with-alpha drawable)))
        (image-width (car (gimp-image-width image)))
        (image-height (car (gimp-image-height image)))
        (active-selection)
        (from-selection)
        (selection-bounds)
        (select-offset-x)
        (select-offset-y)
        (select-width)
        (select-height)
        (effect-layer)
        (active-layer)
        )

    (gimp-context-push) 
    (gimp-image-undo-group-start image)

    (gimp-layer-add-alpha drawable)
    
    (if (= (car (gimp-selection-is-empty image)) TRUE)
        (begin
          (gimp-selection-layer-alpha drawable)
          (set! active-selection (car (gimp-selection-save image)))
          (set! from-selection FALSE))
        (begin
          (set! from-selection TRUE)
          (set! active-selection (car (gimp-selection-save image)))))
    
    (set! selection-bounds (gimp-selection-bounds image))
    (set! select-offset-x (cadr selection-bounds))
    (set! select-offset-y (caddr selection-bounds))
    (set! select-width (- (cadr (cddr selection-bounds)) select-offset-x))
    (set! select-height (- (caddr (cddr selection-bounds)) select-offset-y))
    
    (if (= separate-layer TRUE)
        (begin
          (set! effect-layer (car (gimp-layer-new image
                                                  select-width
                                                  select-height
                                                  type
                                                  "effect layer"
                                                  100
                                                  NORMAL-MODE)))
          
          (gimp-image-add-layer image effect-layer -1)
          (gimp-layer-set-offsets effect-layer select-offset-x select-offset-y)
          (gimp-selection-none image)
          (gimp-edit-clear effect-layer)
          (gimp-selection-load active-selection)
          (gimp-edit-copy drawable)
          
          (let ((floating-sel (car (gimp-edit-paste effect-layer FALSE))))
            (gimp-floating-sel-anchor floating-sel)
            )
          (gimp-image-set-active-layer image effect-layer ))
          (set! effect-layer drawable)
    )
    (set! active-layer effect-layer)

    (if (and
         (= remove-bg TRUE)
         (= old-bg '(0 0 0)))
        (gimp-context-set-foreground '(0 0 0))
        (gimp-context-set-foreground '(14 14 14)))
    
    (gimp-selection-load active-selection)
    (plug-in-maze 1 image active-layer 5 5 TRUE 0 seed 57 1)
    (plug-in-oilify 1 image active-layer mask-size 0)
    (plug-in-edge 1 image active-layer 2 1 0)
    (if (= type RGBA-IMAGE)
      (gimp-desaturate active-layer))
    
    (if (and
         (= remove-bg TRUE)
         (= separate-layer TRUE))
        (begin
          (gimp-by-color-select
           active-layer
           '(0 0 0)
           15
           2
           TRUE
           FALSE
           10
           FALSE)
          (gimp-edit-clear active-layer)))
    
    (if (= keep-selection FALSE)
        (gimp-selection-none image))
    
    (gimp-image-remove-channel image active-selection)
    (gimp-image-set-active-layer image drawable)

    (gimp-image-undo-group-end image)
    (gimp-context-pop)

    (gimp-displays-flush)
  )
)

(tiny-fu-register "tiny-fu-circuit"
    _"_Circuit..."
    "Fills the current selection with something that looks 
    vaguely like a circuit board."
    "Adrian Likins <adrian@gimp.org>"
    "Adrian Likins"
    "10/17/97"
    "RGB* GRAY*"
    SF-IMAGE      "Image" 0
    SF-DRAWABLE   "Drawable" 0
    SF-ADJUSTMENT _"Oilify mask size" '(17 3 50 1 10 0 1)
    SF-ADJUSTMENT _"Circuit seed" '(3 1 3000000 1 10 0 1)
    SF-TOGGLE     _"No background (only for separate layer)" FALSE
    SF-TOGGLE     _"Keep selection" TRUE
    SF-TOGGLE     _"Separate layer" TRUE
)

(tiny-fu-menu-register "tiny-fu-circuit"
                       "<Image>/Tiny-Fu/Render")
