<template>
  <div id="infobox" :style='{opacity}'>
    <div :style='{opacity: bioOpacity}'>
      <div class='year'><strong>{{ parseInt(year) }}</strong></div>
      <div class='title'>{{ name }}</div>
      <div class='fields'><em>{{ fields }}</em></div>
      <div>{{ shortSummary }}</div>
      <p>
        <a :src='url' target='_new'>read more</a> →
      </p>
    </div>
  </div>
</template>

<script>
import _ from 'lodash'
import * as d3 from 'd3'

export default {
  name: 'infobox',
  props: ['tl', 'orbs'],
  data() {
    return {
      name: '',
      fields: '',
      year: 1940,
      shortSummary: '',
      opacity: 0,
      bioOpacity: 0,
      url: '',
    }
  },
  mounted() {
    this.createTimeline()
  },
  watch: {
    orbs() {
      this.createTimeline()
    }
  },
  methods: {
    createTimeline() {
      if (!this.orbs.length) return

      const bioDuration = 0.2

      this.tl.to(this.$data, {opacity: 1}, 0)
      _.each(this.orbs, ({name, url, fields, year, shortSummary}, i) => {
        // year
        this.tl.to(this.$data, {year}, i)

        // fade out prev
        this.tl.to(this.$data, {bioOpacity: 0, duration: bioDuration / 2}, i)
        // update info
        this.tl.add(() => {
          this.name = name
          this.fields = fields
          this.shortSummary = shortSummary
          this.url = url
        }, i + 0.5)
        // fade it back in
        this.tl.to(this.$data, {bioOpacity: 1, duration: bioDuration}, i + 0.5)
      })

      this.tl.to(this.$data, {opacity: 0}, this.orbs.length)
    },
  }
}
</script>

<style scoped>
#infobox {
  position: absolute;
  top: 50vh;
  left: 50vw;
  width: 300px;
  transform: translate(-50%, -50%);
  text-align: center;
  color: #0B1E38;
}

.title {
  font-size: 1.5em;
  font-weight: bold;
}

.fields {
  font-size: 0.85em;
  white-space: nowrap;
}

.year {
  display: inline-block;
  font-size: 1.25em;
  font-weight: bold;
  border-bottom: 1px solid;
}
</style>
