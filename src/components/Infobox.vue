<template>
  <div :style='{opacity}'>
    <div class='infobox' v-if='bioOpacity' :style='{opacity: bioOpacity}'>
      <div class='year'><strong>{{ parseInt(year) }}</strong></div>
      <div class='title'>{{ name }}</div>
      <div class='fields'><em>{{ fields }}</em></div>
      <div>{{ shortSummary }}</div>
      <p>
        <a :href='url' target='_new'>read more</a> →
      </p>
    </div>
    <div class='outro' v-if='outroOpacity' :style='{opacity: outroOpacity}'>
      <p>
        <iframe width="560" height="315" src="https://www.youtube.com/embed/bEM0CRdCrQo" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
      </p>
      <p>
        <strong>Read more about One Amongst Many here:</strong><br />
        <a href='http://www.cdacanay.com/itp-blog/2019/12/23/one-amongst-many-connecting-womxn-in-computing' target='_new'>
          Christina's design-centric recounting
        </a><br />
        <a href='https://tina.pizza/one-amongst-many' target='_new'>
          Tina's physical computing story
        </a><br />
        <a href='http://www.datasketch.es/june/' target='_new'>
          Shirley's data visualization write-up
        </a>
      </p>

      <p style='font-size: 0.85em'>
        <em>Made with love in Brooklyn, 2019.</em>
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
      outroOpacity: 0,
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

      this.tl.to(this.$data, {bioOpacity: 0}, this.orbs.length)

      this.tl.to(this.$data, {outroOpacity: 1}, this.orbs.length + 0.5)
    },
  }
}
</script>

<style scoped>
.infobox {
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
}

.year {
  display: inline-block;
  font-size: 1.25em;
  font-weight: bold;
  border-bottom: 1px solid;
}

.outro {
  position: absolute;
  top: 50vh;
  left: 50vw;
  width: 600px;
  transform: translate(-50%, -50%);
  text-align: center;
}
</style>
