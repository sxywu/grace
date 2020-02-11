<template>
  <div id="app">
    <World v-bind='{width, height, maxZPosition, orbs}' />
  </div>
</template>

<script>
import _ from 'lodash'
import * as d3 from 'd3'
import data from '../data/20names.json'
const filtered = _.filter(data, d => d.year >= 1900)

import World from './components/World.vue'

export default {
  name: 'app',
  components: {World},
  data() {
    return {
      width: window.innerWidth,
      height: window.innerHeight,
      yellow: '#ffed4f', blue: '#0B1E38',
      maxZPosition: filtered.length,
      orbs: [],
    }
  },
  mounted() {
    const yScale = d3.scaleLinear()
      .domain(d3.extent(filtered, d => d.backlinks)).range([-1, 1])
    const zScale = d3.scaleLinear()
      .domain(d3.extent(filtered, d => d.year)).range([0, -this.maxZPosition])
    this.orbs = _.map(filtered, (d, i) => Object.assign(d, {
      x: 0,
      fy: zScale(d.year), // y & z are backwards bc force simulation
      height: yScale(d.backlinks),
    }))
    const simulation = d3.forceSimulation(this.orbs)
      .force('collide', d3.forceCollide(1))
      .force('x', d3.forceX(0))
    _.times(1000, i => simulation.tick())
    _.each(this.orbs, d => Object.assign(d, {y: d.height, z: d.y})) // switch y & z back

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
    handleScroll() {

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

svg {
  overflow: visible;
}

.labels {
  position: absolute;
  top: 0;
  height: 100vh;
}

.beforeAfter {
  position: absolute;
  top: 50%;
  padding: 40px;
  width: calc(100vw - 80px);
  text-align: center;
  transform: translate(0, -50%);
}

.label {
  position: absolute;
  transform: translate(-50%, 0);
  text-align: center;
  padding: 40px;
}
</style>
