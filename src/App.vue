<template>
  <div id="app">
    <!-- CONTENT -->
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
          <circle :r='4' />
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
      <!-- TITLE PAGE -->
      <div class='beforeAfter' style='left:0'>
        <h1>One Amongst Many</h1>
        <p>
          Womxn entering fields dominated by men often feel alone, like there is no history of people like them in these spaces.  This is an issue of storytelling, not of history.
        </p>
        <p>
          <strong>Scroll</strong> through and read about women in computing whose work has been diminished or erased.
        </p>
        <p>
          <em>There have always been role models to be discovered.</em>
        </p>
        <br /><br />
        <h3>↓</h3>
      </div>

      <div class='label' v-for='d in stars' :style='{
          left: d.x, top: d.y + d.scale,
          width: perWidth - margin.left - margin.right,
        }'>
        <h2><a :href='d.url' target='_new'>{{ d.name }}</a></h2>
        <em>{{ d.fields }}</em>
        <p>{{ d.shortSummary }}</p>
        <h3>{{ d.year }}</h3>
      </div>

      <div class='beforeAfter' :style='{left: `${stars.length * perWidth + width - margin.left}px`}'>
        <svg width='200' height='200'>
          <g v-for='d in usStars' :transform='`translate(${d.x}, ${d.y})`'>
            <path :d='d.path' :fill='blue' :stroke='yellow' :stroke-width='4 / d.scale'
              :transform='`scale(${d.scale})rotate(${d.rotate + (twinkle ? d.rotate : 0)})`' />
            <circle :r='d.scale' :fill='yellow' style='filter: url(#bigBlur)' />
          </g>
        </svg>
        <p>
          made with love in brooklyn<br />
          <a href='http://cdacanay.com/' target='_new'>christina dacanay</a><br />
          <a href='http://tina.pizza/' target='_new'>tina rungsawang</a><br />
          <a href='http://sxywu.com/' target='_new'>shirley wu</a>
        </p>
      </div>
    </div>
  </div>
</template>

<script>
import _ from 'lodash'
import {scaleLinear, extent, timer} from 'd3'
import {randomNormal} from 'd3-random'
import data from '../data/20names.json'

const width = window.innerWidth
const height = window.innerHeight
const margin = {top: 150, right: 40, bottom: 60, left: 40}
const perWidth = 700

export default {
  name: 'app',
  data() {
    return {
      width, height,
      perWidth,
      totalWidth: (data.length + 3) * perWidth + 2 * width,
      margin,
      yellow: '#ffed4f', blue: '#020429',
      stars: [],
      bgStars: [],
      usStars: [],
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
        x: (i + 0.5) * perWidth + width,
        y: yScale(d.backlinks),
        rotate: _.random(20, 40),
        scale: 40,
        path: this.starPath(),
      })).value()

    this.bgStars = _.times(150, i => {
      return {
        x: _.random(this.totalWidth),
        y: _.random(this.height),
        r: _.random(6, 12),
      }
    })

    const initialAngle = _.random(Math.PI / 2)
    this.usStars = _.times(3, i => {
      const angle = (i / 1.5) * Math.PI + initialAngle
      const scale = 30
      const x = 1.5 * scale * Math.cos(angle) + 100
      const y = 2 * scale * Math.sin(angle) + 100
      return {
        x, y, scale,
        rotate: _.random(20, 40),
        path: this.starPath(),
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
        const x = randomNormal(radius * Math.cos(angle), 0.05)()
        const y = randomNormal(radius * Math.sin(angle), 0.05)()

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
