<template>
  <div id="intro" v-if='opacity' :style='{opacity}'>
    <div class='videos'>
      <video src='https://storage.googleapis.com/one-amongst-many-v2/timelapse_reduced.mp4'
         :style='{opacity: timelapseOpacity}' preload autoplay loop muted playsinline></video>
      <video src='https://storage.googleapis.com/one-amongst-many-v2/fancy_reduced.mp4'
        :style='{opacity: fancyOpacity}' preload autoplay loop muted playsinline></video>
      <div class='overlayImage'></div>
    </div>
    <div class='subsection' :style='{opacity: scrollOpacity}'>
      <div class='title'>One Amongst Many</div>
      <div class='byline'>
        by <a href='http://cdacanay.com/' target='_new'>Christina Dacanay</a>, <a href='https://tina.pizza/' target='_new'>Tina Rungsawang</a>, and <a href='http://sxywu.com/' target='_new'>Shirley Wu</a>
      </div>
      <p>
        Young women entering fields dominated by men often feel like there is no history of people like them in their field. We know now that this is an issue of storytelling, not of history. Women have been contributing to every field, however invisibly, since the beginning of time.
      </p>
      <p>
        One Amongst Many attempts to illuminate the histories of women in computing that have been diminished or erased. It is a data installation where each woman is arranged in a field by the year of her greatest achievement, and the height of the orb correlated to her renown. Every orb starts dimmed, and gets brighter each time another person reads about them, literally shedding light on the woman’s accomplishments.
      </p>
    </div>
    <div class='subsection' :style='{opacity: legendOpacity}'>
      <p>
        One Amongst Many is a physical data visualization created at New York University’s ITP Master’s program. The original installation consisted of 16-20 illuminated orbs suspended from the ceiling, each with a woman’s biography inside. This website is a digital analog to the installation, so that people around the world can learn about these incredible women in computing.
      </p>
    </div>
    <div class='arrow'>⌄</div>
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

.title {
  font-size: 2em;
  font-weight: bold;
  text-align: center;
}

.byline {
  font-size: 0.85em;
  text-align: center;
}

.subsection {
  position: absolute;
  top: 40vh;
  left: 50vw;
  width: 600px;
  transform: translate(-50%, -50%);
  text-align: justify;
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

.arrow {
  position: absolute;
  bottom: 80px;
  width: 100vw;
  text-align: center;
  font-size: 3em;
  animation: bounce 1.5s ease-in-out infinite;
}

@keyframes bounce {
  0%  { transform: translateY(0)   }
  50% { transform: translateY(-20px)}
  100% { transform: translateY(0)   }
}

@media (min-width: 320px) and (max-width: 480px) {
  .subsection {
    top: 45vh;
    left: 0px;
    width: calc(100% - 40px);
    padding: 20px 20px 20px 20px;
    transform: translate(0%, -50%);
  }

  .arrow {
    bottom: 10px;
  }
}
</style>
