## Process this file with automake to produce Makefile.in

SUBDIRS = images

scriptdatadir = $(GIMP_DATADIR)/scripts

scripts =  \
    tiny-fu.init           \
    script-fu-compat.init  \
    3d-outline.scm         \
    3dTruchet.scm          \
    add-bevel.scm          \
    addborder.scm          \
    alien-glow-arrow.scm   \
    alien-glow-bar.scm     \
    alien-glow-bullet.scm  \
    alien-glow-button.scm  \
    alien-glow-logo.scm    \
    alien-neon-logo.scm    \
    basic1-logo.scm        \
    basic2-logo.scm        \
    beveled-button.scm     \
    beveled-pattern-arrow.scm   \
    beveled-pattern-bullet.scm  \
    beveled-pattern-button.scm  \
    beveled-pattern-heading.scm \
    beveled-pattern-hrule.scm   \
    blend-anim.scm          \
    blended-logo.scm        \
    bovinated-logo.scm      \
    burn-in-anim.scm        \
    camo.scm                \
    carved-logo.scm         \
    carve-it.scm            \
    chalk.scm               \
    chip-away.scm           \
    chrome-it.scm           \
    chrome-logo.scm         \
    circuit.scm             \
    clothify.scm            \
    coffee.scm              \
    comic-logo.scm          \
    contactsheet.scm        \
    coolmetal-logo.scm      \
    copy-visible.scm        \
    crystal-logo.scm        \
    distress-selection.scm  \
    drop-shadow.scm         \
    erase-rows.scm          \
    fade-outline.scm        \
    flatland.scm            \
    font-map.scm            \
    frosty-logo.scm         \
    fuzzyborder.scm         \
    gimp-headers.scm        \
    gimp-labels.scm         \
    glossy.scm              \
    glowing-logo.scm        \
    gradient-bevel-logo.scm \
    gradient-example.scm    \
    grid-system.scm         \
    guides-from-selection.scm \
    guides-new-percent.scm  \
    guides-new.scm          \
    guides-remove-all.scm   \
    hsv-graph.scm           \
    i26-gunya2.scm          \
    land.scm                \
    lava.scm                \
    line-nova.scm           \
    mkbrush.scm             \
    neon-logo.scm           \
    news-text.scm           \
    old-photo.scm           \
    paste-as-brush.scm      \
    paste-as-pattern.scm    \
    perspective-shadow.scm  \
    predator.scm            \
    pupi-button.scm         \
    rendermap.scm           \
    reverse-layers.scm      \
    ripply-anim.scm         \
    round-corners.scm       \
    selection-round.scm     \
    select-to-brush.scm     \
    select-to-image.scm     \
    select-to-pattern.scm   \
    slide.scm               \
    sota-chrome-logo.scm    \
    speed-text.scm          \
    sphere.scm              \
    spinning-globe.scm      \
    spyrogimp.scm           \
    starburst-logo.scm      \
    starscape-logo.scm      \
    swirltile.scm           \
    swirly-pattern.scm      \
    text-circle.scm         \
    textured-logo.scm       \
    tileblur.scm            \
    tiny-fu-util.scm        \
    tiny-fu-set-cmap.scm    \
    title-header.scm        \
    t-o-p-logo.scm          \
    truchet.scm             \
    unsharp-mask.scm        \
    waves-anim.scm          \
    weave.scm               \
    web-browser.scm         \
    xach-effect.scm

test_scripts = \
    test-sphere.scm   \
    ts-helloworld.scm

 
if TINY_FU_UNSTABLE
scriptdata_DATA = $(scripts) $(test_scripts)
else
scriptdata_DATA = $(scripts)
endif


EXTRA_DIST = $(scripts) $(test_scripts)
