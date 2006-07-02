## Process this file with automake to produce Makefile.in

SUBDIRS = images

scriptdatadir = $(GIMP_DATADIR)/scripts

scripts =  \
    tiny-fu.init           \
    script-fu-compat.init  \
    3d-outline.sct         \
    3dTruchet.sct          \
    add-bevel.sct          \
    addborder.sct          \
    alien-glow-arrow.sct   \
    alien-glow-bar.sct     \
    alien-glow-bullet.sct  \
    alien-glow-button.sct  \
    alien-glow-logo.sct    \
    alien-neon-logo.sct    \
    basic1-logo.sct        \
    basic2-logo.sct        \
    beveled-button.sct     \
    beveled-pattern-arrow.sct   \
    beveled-pattern-bullet.sct  \
    beveled-pattern-button.sct  \
    beveled-pattern-heading.sct \
    beveled-pattern-hrule.sct   \
    blend-anim.sct          \
    blended-logo.sct        \
    bovinated-logo.sct      \
    burn-in-anim.sct        \
    camo.sct                \
    carved-logo.sct         \
    carve-it.sct            \
    chalk.sct               \
    chip-away.sct           \
    chrome-it.sct           \
    chrome-logo.sct         \
    circuit.sct             \
    clothify.sct            \
    coffee.sct              \
    comic-logo.sct          \
    contactsheet.sct        \
    coolmetal-logo.sct      \
    copy-visible.sct        \
    crystal-logo.sct        \
    distress-selection.sct  \
    drop-shadow.sct         \
    erase-rows.sct          \
    fade-outline.sct        \
    flatland.sct            \
    font-map.sct            \
    frosty-logo.sct         \
    fuzzyborder.sct         \
    gimp-headers.sct        \
    gimp-labels.sct         \
    glossy.sct              \
    glowing-logo.sct        \
    gradient-bevel-logo.sct \
    gradient-example.sct    \
    grid-system.sct         \
    guides-from-selection.sct \
    guides-new-percent.sct  \
    guides-new.sct          \
    guides-remove-all.sct   \
    hsv-graph.sct           \
    i26-gunya2.sct          \
    land.sct                \
    lava.sct                \
    line-nova.sct           \
    mkbrush.sct             \
    neon-logo.sct           \
    news-text.sct           \
    old-photo.sct           \
    paste-as-brush.sct      \
    paste-as-pattern.sct    \
    perspective-shadow.sct  \
    predator.sct            \
    pupi-button.sct         \
    rendermap.sct           \
    reverse-layers.sct      \
    ripply-anim.sct         \
    round-corners.sct       \
    selection-round.sct     \
    select-to-brush.sct     \
    select-to-image.sct     \
    select-to-pattern.sct   \
    slide.sct               \
    sota-chrome-logo.sct    \
    speed-text.sct          \
    sphere.sct              \
    spinning-globe.sct      \
    spyrogimp.sct           \
    starburst-logo.sct      \
    starscape-logo.sct      \
    swirltile.sct           \
    swirly-pattern.sct      \
    text-circle.sct         \
    textured-logo.sct       \
    tileblur.sct            \
    tiny-fu-util.sct        \
    tiny-fu-set-cmap.sct    \
    title-header.sct        \
    t-o-p-logo.sct          \
    truchet.sct             \
    unsharp-mask.sct        \
    waves-anim.sct          \
    weave.sct               \
    web-browser.sct         \
    xach-effect.sct

test_scripts = \
    test-sphere.sct   \
    ts-helloworld.sct

 
if TINY_FU_UNSTABLE
scriptdata_DATA = $(scripts) $(test_scripts)
else
scriptdata_DATA = $(scripts)
endif


EXTRA_DIST = $(scripts) $(test_scripts)
