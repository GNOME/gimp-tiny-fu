;;; unsharp-mask.scm
;;; Time-stamp: <1998/11/17 13:18:39 narazaki@gimp.org>
;;; Author: Narazaki Shuji <narazaki@gimp.org>
;;; Version 0.8

(define (script-fu-unsharp-mask img drw mask-size mask-opacity)
  (let* (
        (drawable-width (car (gimp-drawable-width drw)))
        (drawable-height (car (gimp-drawable-height drw)))
        (new-image (car (gimp-image-new drawable-width drawable-height RGB)))
        (original-layer (car (gimp-layer-new new-image
                                             drawable-width drawable-height
                                             RGB-IMAGE "Original"
                                             100 NORMAL-MODE)))
        (original-layer-for-darker)
        (original-layer-for-lighter)
        (blured-layer-for-darker)
        (blured-layer-for-lighter)
        (darker-layer)
        (lighter-layer)
        )

    (gimp-selection-all img)
    (gimp-edit-copy drw)

    (gimp-image-undo-disable new-image)

    (gimp-image-add-layer new-image original-layer 0)
    (gimp-floating-sel-anchor
      (car (gimp-edit-paste original-layer FALSE)))

    (set! original-layer-for-darker (car (gimp-layer-copy original-layer TRUE)))
    (set! original-layer-for-lighter (car (gimp-layer-copy original-layer TRUE)))
    (set! blured-layer-for-darker (car (gimp-layer-copy original-layer TRUE)))
    (gimp-drawable-set-visible original-layer FALSE)
    (gimp-display-new new-image)

    ;; make darker mask
    (gimp-image-add-layer new-image blured-layer-for-darker -1)
    (plug-in-gauss-iir RUN-NONINTERACTIVE
		       new-image blured-layer-for-darker mask-size TRUE TRUE)
    (set! blured-layer-for-lighter
          (car (gimp-layer-copy blured-layer-for-darker TRUE)))
    (gimp-image-add-layer new-image original-layer-for-darker -1)
    (gimp-layer-set-mode original-layer-for-darker SUBTRACT-MODE)
    (set! darker-layer
          (car (gimp-image-merge-visible-layers new-image CLIP-TO-IMAGE)))
    (gimp-drawable-set-name darker-layer "darker mask")
    (gimp-drawable-set-visible darker-layer FALSE)

    ;; make lighter mask
    (gimp-image-add-layer new-image original-layer-for-lighter -1)
    (gimp-image-add-layer new-image blured-layer-for-lighter -1)
    (gimp-layer-set-mode blured-layer-for-lighter SUBTRACT-MODE)
    (set! lighter-layer
          (car (gimp-image-merge-visible-layers new-image CLIP-TO-IMAGE)))
    (gimp-drawable-set-name lighter-layer "lighter mask")

    ;; combine them
    (gimp-drawable-set-visible original-layer TRUE)
    (gimp-layer-set-mode darker-layer SUBTRACT-MODE)
    (gimp-layer-set-opacity darker-layer mask-opacity)
    (gimp-drawable-set-visible darker-layer TRUE)
    (gimp-layer-set-mode lighter-layer ADDITION-MODE)
    (gimp-layer-set-opacity lighter-layer mask-opacity)
    (gimp-drawable-set-visible lighter-layer TRUE)

    (gimp-image-undo-enable new-image)
    (gimp-displays-flush)
  )
)

(script-fu-register "script-fu-unsharp-mask"
  "Unsharp Mask..."
  "Make a new image from the current layer by applying the unsharp mask method"
  "Shuji Narazaki <narazaki@gimp.org>"
  "Shuji Narazaki"
  "1997,1998"
  ""
  SF-IMAGE      "Image"             0
  SF-DRAWABLE   "Drawable to apply" 0
  SF-ADJUSTMENT _"Mask size"        '(5 1 100 1 1 0 1)
  SF-ADJUSTMENT _"Mask opacity"     '(50 0 100 1 1 0 1)
)
