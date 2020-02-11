<template>
  <div id="intro" :style='{display}'>
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
  props: ['tl'],
  data() {
    return {
      display: 'block',
      scrollOpacity: 1,
      legendOpacity: 0,
    }
  },
  mounted() {
    // register to timeline
    // 1. fade out scroll
    this.tl.to(this.$data, {scrollOpacity: 0})
    // 2. fade in legend
    this.tl.to(this.$data, {legendOpacity: 1})
    // 3. fade out legend
    this.tl.to(this.$data, {legendOpacity: 0})
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
</style>
