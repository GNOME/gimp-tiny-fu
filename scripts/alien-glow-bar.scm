; The GIMP -- an image manipulation program
; Copyright (C) 1995 Spencer Kimball and Peter Mattis
;
; Alien Glow themed hrule for web pages
; Copyright (c) 1997 Adrian Likins
; aklikins@eos.ncsu.edu
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

(define (script-fu-alien-glow-horizontal-ruler length
                                               height
                                               glow-color
                                               bg-color
                                               flatten)
  (let* (
        (img (car (gimp-image-new height length RGB)))
        (border (/ height 4))
        (ruler-layer (car (gimp-layer-new img
                                          (+ length height) (+ height height)
                                          RGBA-IMAGE "Ruler" 100 NORMAL-MODE)))
        (glow-layer (car (gimp-layer-new img
                                         (+ length height) (+ height height)
                                         RGBA-IMAGE "ALien Glow" 100 NORMAL-MODE)))
        (bg-layer (car (gimp-layer-new img
                                       (+ length height) (+ height height)
                                       RGB-IMAGE "Backround" 100 NORMAL-MODE)))
        )

    (gimp-context-push)
    (gimp-image-undo-disable img)
    (gimp-image-resize img (+ length height) (+ height height) 0 0)
    (gimp-image-add-layer img bg-layer 1)
    (gimp-image-add-layer img glow-layer -1)
    (gimp-image-add-layer img ruler-layer -1)

   ; (gimp-layer-set-lock-alpha ruler-layer TRUE)
    (gimp-context-set-background bg-color)
    (gimp-edit-fill bg-layer BACKGROUND-FILL)
    (gimp-edit-clear glow-layer)
    (gimp-edit-clear ruler-layer)

    (gimp-rect-select img (/ height 2) (/ height 2) length height CHANNEL-OP-REPLACE FALSE 0)
    (gimp-context-set-foreground '(79 79 79))
    (gimp-context-set-background '(0 0 0))

    (gimp-edit-blend ruler-layer FG-BG-RGB-MODE NORMAL-MODE
                     GRADIENT-SHAPEBURST-ANGULAR 100 0 REPEAT-NONE FALSE
                     FALSE 0 0 TRUE
                     0 0 height height)

    (gimp-context-set-foreground glow-color)
    (gimp-selection-grow img border)
    (gimp-edit-fill glow-layer FOREGROUND-FILL)
    (gimp-selection-none img)
    (plug-in-gauss-rle 1 img glow-layer 25 TRUE TRUE)

    (gimp-image-undo-enable img)

    (if (= flatten TRUE)
        (gimp-image-flatten img))

    (gimp-context-pop)
    (gimp-display-new img)
  )
)


(script-fu-register "script-fu-alien-glow-horizontal-ruler"
    _"_Hrule..."
    _"Create an Hrule graphic with an eerie glow for web pages"
    "Adrian Likins"
    "Adrian Likins"
    "1997"
    ""
    SF-ADJUSTMENT _"Bar length"       '(480 5 1500 1 10 0 1)
    SF-ADJUSTMENT _"Bar height"       '(16 1 100 1 10 0 1)
    SF-COLOR      _"Glow color"       '(63 252 0)
    SF-COLOR      _"Background color" "black"
    SF-TOGGLE     _"Flatten image"    TRUE
)

(script-fu-menu-register "script-fu-alien-glow-horizontal-ruler"
                         "<Toolbox>/Xtns/Web Page Themes/Alien Glow")
