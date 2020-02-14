<template>
  <div id="intro" :style='{display, opacity}'>
    <div class='videos'>
      <video src='https://storage.googleapis.com/one-amongst-many-v2/timelapse_reduced.mp4'
         :style='{opacity: timelapseOpacity}' preload autoplay loop muted playsinline></video>
      <video src='https://storage.googleapis.com/one-amongst-many-v2/fancy_reduced.mp4'
        :style='{opacity: fancyOpacity}' preload autoplay loop muted playsinline></video>
      <div class='overlayImage'></div>
    </div>
    <div class='subsection' :style='{opacity: scrollOpacity}'>
      <h1>One Amongst Many</h1>
      <p>
        is intended to be a physical data installation, but it is important to us to be able to share these amazing women's contributions with the world.
      </p>
      <p>
        Scroll to explore and read about their accomplishments.
      </p>
      ↓
    </div>
    <div class='subsection' :style='{opacity: legendOpacity}'>
      <p>
        In this digital counterpart, the women are positioned by the year of their accomplishment, and their heights are the level of their renown.
      </p>
    </div>
  </div>
</template>

<script>
import _ from 'lodash'
import * as d3 from 'd3'

export default {
  name: 'intro',
  props: ['tl', 'width', 'height'],
  data() {
    return {
      display: 'block',
      opacity: 1,
      scrollOpacity: 1,
      legendOpacity: 0,
      fancyOpacity: 1,
      timelapseOpacity: 0,
    }
  },
  mounted() {
    // register to timeline
    // 1. fade out scroll
    this.tl.to(this.$data, {scrollOpacity: 0}, 1)
    // 2. fade in legend
    this.tl.to(this.$data, {fancyOpacity: 0, legendOpacity: 1, timelapseOpacity: 1}, 1.5)
    // 3. fade out legend
    this.tl.to(this.$data, {legendOpacity: 0, timelapseOpacity: 0, opacity: 0, duration: 1.5}, 2.5)
    this.tl.set(this.$data, {display: 'none'}, 4)
  },
  watch: {
  },
  methods: {
  }
}
</script>

<style scoped>
#intro {
  position: absolute;
  top: 0;
  left: 0;
  width: 100vw;
  height: 100vh;
}

.subsection {
  position: absolute;
  top: 40vh;
  left: 50vw;
  width: 600px;
  transform: translate(-50%, -50%);
}

.videos {
  position: absolute;
  top: 0;
  left: 0;
  width: 100vw;
  height: 100vh;
  overflow: hidden;
}

.videos video {
  position: absolute;
  top: 50%;
  left: 50%;
  width: auto;
  min-width: 100vw;
  height: auto;
  min-height: 100vh;
  transform: translateX(-50%) translateY(-50%);
}

.overlayImage {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-image: url(https://storage.googleapis.com/one-amongst-many-v2/bg-pattern.png);
  opacity: .3;
  pointer-events: none;
}
</style>
