<template>
  <div id="app">
    <svg :width='width' :height='height'>
      <defs>
        <filter id='smallBlur' width='400%' height='400%' x='-100%' y='-100%'>
          <feGaussianBlur in='SourceGraphic' stdDeviation='12' />
        </filter>
        <filter id='bigBlur' width='400%' height='400%' x='-100%' y='-100%'>
          <feGaussianBlur in='SourceGraphic' stdDeviation='45' />
        </filter>
      </defs>
      <g :transform='`translate(${-scrollX}, 0)`'>
        <g v-for='(d, i) in bgStars' :transform='`translate(${d.x}, ${d.y})`'
          :fill='yellow' :opacity='(Math.sin(elapsed * 0.003 + i) + 1) / 2'>
          <circle :r='d.r' style='filter: url(#smallBlur)' />
          <circle :r='3' />
        </g>
        <g v-for='d in stars' :transform='`translate(${d.x}, ${d.y})`'>
          <line :y2='-height' :stroke='yellow' stroke-dasharray='10' />
          <path :d='d.path' :fill='blue' :stroke='yellow' :stroke-width='4 / d.scale'
            :transform='`scale(${d.scale})rotate(${d.rotate + (twinkle ? d.rotate : 0)})`' />
          <circle :r='d.scale' :fill='yellow' style='filter: url(#bigBlur)' />
        </g>
      </g>
    </svg>
    <div class='labels' :style='{left: `${-scrollX}px`}'>
      <div class='label' v-for='d in stars' :style='{
          left: d.x, top: d.y + d.scale,
          width: perWidth - margin.left - margin.right,
        }'>
        <h2><a :href='d.url' target='_new'>{{ d.name }}</a></h2>
        <em>{{ d.fields }}</em>
        <p>{{ d.shortSummary }}</p>
        <h3>{{ d.year }}</h3>
      </div>
    </div>
  </div>
</template>

<script>
import _ from 'lodash'
import {scaleLinear, extent, timer} from 'd3'
import p5 from 'p5'
import data from '../data/20names.json'

const margin = {top: 150, right: 40, bottom: 60, left: 40}
const perWidth = 700

export default {
  name: 'app',
  data() {
    return {
      width: window.innerWidth,
      height: window.innerHeight,
      perWidth,
      totalWidth: (data.length + 2.5) * perWidth,
      margin,
      yellow: '#ffed4f', blue: '#020429',
      stars: [],
      bgStars: [],
      twinkle: 0,
      scrollX: 0,
      elapsed: 0,
    }
  },
  mounted() {
    const yScale = scaleLinear()
      .domain(extent(data, d => d.backlinks))
      .range([this.height / 2, margin.top])

    this.stars = _.chain(data)
      .sortBy(d => d.year)
      .map((d, i) => Object.assign(d, {
        x: (i + 0.5) * perWidth,
        y: yScale(d.backlinks),
        rotate: _.random(20, 40),
        scale: 40,
        path: this.starPath(),
      })).value()

    this.bgStars = _.times(150, i => {
      return {
        x: _.random(this.totalWidth),
        y: _.random(this.height),
        r: _.random(4, 10),
      }
    })

    // scroll
    window.addEventListener('scroll', this.handleScroll)
    this.handleScroll()

    // twinkle
    this.twinkleStars()
  },
  destroyed() {
    window.removeEventListener('scroll', this.handleScroll)
  },
  methods: {
    twinkleStars() {
      let prev1 = 0
      let prev2 = 0
      timer(elapsed => {
        if (elapsed - prev1 > 400) {
          this.twinkle = !this.twinkle
          prev1 = elapsed
        }
        if (elapsed - prev2 > 200) {
          this.elapsed = elapsed
          prev2 = elapsed
        }
      })
    },
    handleScroll() {
      const scrollTop = window.scrollY || document.documentElement.scrollTop || document.body.scrollTop || 0
      this.scrollX = (scrollTop / 10000) * this.totalWidth
    },
    starPath() {
      const outerRadius = 1
      const innerRadius = 0.5
      let path = ''
      _.times(30, i => {
        const radius = i % 2 ? outerRadius : innerRadius
        const angle = i * (Math.PI / 5)
        const command = i === 0 ? 'M' : 'L'
        const x = p5.prototype.randomGaussian(radius * Math.cos(angle), 0.05)
        const y = p5.prototype.randomGaussian(radius * Math.sin(angle), 0.05)

        path += `${command} ${x},${y}`
      })
      return `${path}Z`
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
}

.label {
  position: absolute;
  transform: translate(-50%, 0);
  text-align: center;
  padding: 40px;
}
</style>
