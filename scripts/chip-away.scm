; The GIMP -- an image manipulation program
; Copyright (C) 1995 Spencer Kimball and Peter Mattis
;
;  Supposed to look vaguely like roughly carved wood. Chipped away if you will.
;
;  Options: Text String - the string to make the logo from
;           Font        - which font to use
;           Font Size   - how big
;           Chip Amount - how rought he chipping is (how spread the bump map is)
;           Blur Amount - the bump layer is blurred slighty by this amount
;           Invert      - whether or not to invert the bumpmap (gives a carved in feel)
;           Drop Shadow - whether or not to draw a drop shadow
;           Keep bump layer? - whether to keep the layer used as the bump map
;           fill bg with pattern? - whether to fill the background with the pattern or leave it white
;           Keep Background - whether or not to remove the background layer
;
;  Adrian Likins  (Adrian@gimp.org)
;  Jan 11, 1998 v1
;
;  see http://www.gimp.org/~adrian/script.html
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
;
;  Some suggested patterns: Dried mud, 3D green, Slate
;

(define (apply-chip-away-logo-effect img
                                     logo-layer
                                     spread-amount
                                     blur-amount
                                     invert
                                     drop-shadow
                                     keep-bump
                                     bg-fill
                                     keep-back
                                     pattern)
  (let* (
        (width (car (gimp-drawable-width logo-layer)))
        (height (car (gimp-drawable-height logo-layer)))
        (bg-layer (car (gimp-layer-new img width height RGB-IMAGE "Background" 100 NORMAL-MODE)))
        (bump-layer (car (gimp-layer-new img width height RGBA-IMAGE "Bump Layer" 100 NORMAL-MODE)))
        )

    (gimp-context-push)
    (tiny-fu-util-image-resize-from-layer img logo-layer)
    (gimp-image-add-layer img bg-layer 1)
    (gimp-layer-set-preserve-trans logo-layer TRUE)
    (gimp-context-set-pattern pattern)

    (gimp-context-set-background '(255 255 255))
    (gimp-selection-all img)

    (if (= bg-fill TRUE)
        (gimp-edit-bucket-fill bg-layer 2 NORMAL-MODE 100 255 FALSE 1 1)
        (gimp-edit-fill bg-layer BACKGROUND-FILL))

    (gimp-selection-all img)
    (gimp-edit-clear bump-layer)
    (gimp-selection-none img)
    (gimp-selection-layer-alpha logo-layer)
    (gimp-edit-fill bump-layer BACKGROUND-FILL)
    (gimp-edit-bucket-fill logo-layer 2 NORMAL-MODE 100 255 FALSE 1 1)
    (gimp-selection-none img)

    (gimp-image-add-layer img bump-layer 1)

    (gimp-layer-set-preserve-trans bump-layer FALSE)
    (plug-in-spread 1 img bump-layer spread-amount spread-amount)
    (gimp-selection-layer-alpha bump-layer)
    (plug-in-gauss-rle 1 img bump-layer blur-amount TRUE TRUE)

    (gimp-selection-none img)

    (plug-in-bump-map 1 img logo-layer bump-layer 135.00 25.0 60 0 0 0 0 TRUE invert 1)

    (gimp-drawable-set-visible bump-layer FALSE)

     (if (= drop-shadow TRUE)
        (begin
          (let* ((shadow-layer (car (gimp-layer-new img width height RGBA-IMAGE "Shadow layer" 100 NORMAL-MODE))))
            (gimp-selection-all img)
            (gimp-edit-clear shadow-layer)
            (gimp-selection-none img)
            (gimp-selection-layer-alpha logo-layer)
            (gimp-context-set-background '(0 0 0))
            (gimp-edit-fill shadow-layer BACKGROUND-FILL)
            (gimp-selection-none img)
            (plug-in-gauss-rle 1 img shadow-layer 5 TRUE TRUE)
            (gimp-image-add-layer img shadow-layer 1)
            (gimp-layer-translate shadow-layer 6 6))))

     (if (= keep-bump FALSE)
         (gimp-image-remove-layer img bump-layer))

     (if (= keep-back FALSE)
         (gimp-image-remove-layer img bg-layer))

    (gimp-context-pop)
  )
)

(define (tiny-fu-chip-away-logo-alpha img
                                      logo-layer
                                      spread-amount
                                      blur-amount
                                      invert
                                      drop-shadow
                                      keep-bump
                                      bg-fill
                                      keep-back
                                      pattern)
  (begin
    (gimp-image-undo-group-start img)
    (apply-chip-away-logo-effect img logo-layer spread-amount blur-amount
                                 invert drop-shadow keep-bump bg-fill
                                 keep-back pattern)
    (gimp-image-undo-group-end img)
    (gimp-displays-flush)
  )
)

(tiny-fu-register "tiny-fu-chip-away-logo-alpha"
    _"<Image>/Tiny-Fu/Alpha to Logo/Chip Awa_y..."
    "Chip away effect"
    "Adrian Likins <adrian@gimp.org>"
    "Adrian Likins <adrian@gimp.org>"
    "1997"
    "RGBA"
    SF-IMAGE      "Image" 0
    SF-DRAWABLE   "Drawable" 0
    SF-ADJUSTMENT _"Chip amount" '(30 0 250 1 10 0 1)
    SF-ADJUSTMENT _"Blur amount" '(3 1 100 1 10 1 0)
    SF-TOGGLE     _"Invert" FALSE
    SF-TOGGLE     _"Drop shadow" TRUE
    SF-TOGGLE     _"Keep bump layer" FALSE
    SF-TOGGLE     _"Fill BG with pattern" TRUE
    SF-TOGGLE     _"Keep background" TRUE
    SF-PATTERN    _"Pattern" "Burlwood"
)


(define (tiny-fu-chip-away-logo text
                                font
                                font-size
                                spread-amount
                                blur-amount
                                invert
                                drop-shadow
                                keep-bump
                                bg-fill
                                keep-back
                                pattern)
  (let* ((img (car (gimp-image-new 256 256 RGB)))
         (text-layer (car (gimp-text-fontname img -1 0 0
                                     text 30 TRUE font-size PIXELS font))))
    (gimp-image-undo-disable img)
    (gimp-drawable-set-name text-layer text)
    (apply-chip-away-logo-effect img text-layer spread-amount blur-amount
                                 invert drop-shadow keep-bump bg-fill
                                 keep-back pattern)
    (gimp-image-undo-enable img)
    (gimp-display-new img)
  )
)

(tiny-fu-register "tiny-fu-chip-away-logo"
    _"<Toolbox>/Xtns/Tiny-Fu/Logos/Chip Awa_y..."
    "Chip away effect"
    "Adrian Likins <adrian@gimp.org>"
    "Adrian Likins <adrian@gimp.org>"
    "1997"
    ""
    SF-STRING _"Text" "Sloth"
    SF-FONT   _"Font" "RoostHeavy"
    SF-ADJUSTMENT _"Font size (pixels)" '(200 2 1000 1 10 0 1)
    SF-ADJUSTMENT _"Chip amount" '(30 0 250 1 10 0 1)
    SF-ADJUSTMENT _"Blur amount" '(3 1 100 1 10 1 0)
    SF-TOGGLE     _"Invert" FALSE
    SF-TOGGLE     _"Drop shadow" TRUE
    SF-TOGGLE     _"Keep bump layer" FALSE
    SF-TOGGLE     _"Fill BG with pattern" TRUE
    SF-TOGGLE     _"Keep background" TRUE
    SF-PATTERN    _"Pattern" "Burlwood"
)

