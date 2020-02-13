<template>
  <div id="app">
    <World v-bind='{
      width, height, maxZPosition,
      tl: timelines[1], orbs
    }' /></World>
    <Intro v-bind='{
      tl: timelines[0],
    }'></Intro>
    <Infobox v-bind='{
      tl: timelines[1], orbs,
    }'></Infobox>
  </div>
</template>

<script>
import _ from 'lodash'
import * as d3 from 'd3'
import gsap from 'gsap'

import data from '../data/20names.json'
const filtered = _.chain(data)
  .filter(d => d.year >= 1900)
  .sortBy(d => d.year)
  .value()

import World from './components/World.vue'
import Intro from './components/Intro.vue'
import Infobox from './components/Infobox.vue'

const docHeight = 30000

export default {
  name: 'app',
  components: {World, Intro, Infobox},
  data() {
    const sectionHeights = [docHeight * 0.15, docHeight * 0.84, docHeight * 0.01]
    return {
      width: window.innerWidth,
      height: window.innerHeight,
      yellow: '#ffed4f', blue: '#0B1E38',
      maxZPosition: 1.5 * filtered.length,
      orbs: [],
      // for scroll, section positions
      sections: [
        {index: 0, height: sectionHeights[0], top: 0, bottom: sectionHeights[0]},
        {
          index: 1,
          height: sectionHeights[1],
          top: sectionHeights[0],
          bottom: sectionHeights[0] + sectionHeights[1],
        },
        {
          index: 2,
          height: sectionHeights[2],
          top: sectionHeights[0] + sectionHeights[1],
          bottom: sectionHeights[0] + sectionHeights[1] + sectionHeights[2],
        },
      ],
      // timelines
      timelines: [
        new gsap.timeline({paused: true}),
        new gsap.timeline({paused: true}),
        new gsap.timeline({paused: true}),
      ],
    }
  },
  mounted() {
    const yScale = d3.scaleLinear()
      .domain(d3.extent(filtered, d => d.backlinks)).range([-1, 1])
    const zScale = d3.scaleLinear()
      .domain(d3.extent(filtered, d => d.year)).range([0, -this.maxZPosition])
    const positions = _.map(filtered, (d, i) => {
      return {
        x: 0,
        fy: zScale(d.year), // y & z are backwards bc force simulation
        height: yScale(d.backlinks),
      }
    })
    const simulation = d3.forceSimulation(positions)
      .force('collide', d3.forceCollide(1.1))
      .force('x', d3.forceX(0))
    _.times(1000, i => simulation.tick())
    this.orbs = _.map(filtered, (d, i) => {
      const {x, y, height} = positions[i]
      return Object.assign({}, d, {
        x, y: height, z: y,
      })
    })

    // scroll
    window.addEventListener('scroll', this.handleScroll)
    window.addEventListener('resize', this.handleWindowResize)
    this.handleScroll()
  },
  destroyed() {
    window.removeEventListener('scroll', this.handleScroll)
    window.removeEventListener('resize', this.handleWindowResize)
  },
  methods: {
    handleScroll: function() {
      const scrollTop = window.scrollY || document.documentElement.scrollTop || document.body.scrollTop || 0
      if (scrollTop > docHeight) return

      const {index, top, height} = _.find(this.sections, ({bottom}) => scrollTop <= bottom)
      const progress = (scrollTop - top) / height

      if (index > 0) {
        this.timelines[0].progress(1)
      }
      if (index > 1) {
        this.timelines[1].progress(1)
      }
      if (index < 2) {
        this.timelines[2].progress(0)
      }
      if (index < 1) {
        this.timelines[1].progress(0)
      }

      const tl = this.timelines[index].progress(progress)
    },
    handleWindowResize: function() {
      this.width = window.innerWidth
      this.height = window.innerHeight
    },
  }
}
</script>

<style scoped>
#app {
  position: fixed;
  top: 0;
  left: 0;
}
</style>
