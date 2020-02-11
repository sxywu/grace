<template>
  <div id='world' :style='{opacity, display}'>
    <div ref='container' :width='width' :height='height'></div>
    <canvas class='hidden' v-for='(d, i) in decades' :ref='`decade${i}`'
      :width='2 * textWidth' :height='2 * textHeight'
      :style='{width: `${textWidth}px`, height: `${textHeight}px`}'></canvas>
  </div>
</template>

<script>
import _ from 'lodash'
import rafLoop from 'raf-loop'
import * as d3 from 'd3'
import * as THREE from 'three'

export default {
  name: 'world',
  props: ['width', 'height', 'maxZPosition', 'orbs'],
  data() {
    return {
      opacity: 1,
      display: 'block',
      textWidth: 820,
      textHeight: 420,
      decades: _.range(8)
    }
  },
  created() {
    this.scene = new THREE.Scene()
    this.camera = new THREE.PerspectiveCamera(45, this.width / this.height, 1, 1000)
    this.renderer = new THREE.WebGLRenderer({antialias: true, alpha: true})

    // WebGL background color
    this.renderer.setClearColor(0xffffff, 0)

    // set renderer size & pixel ratio
    this.renderer.setSize(this.width, this.height)
    const dpr = Math.min(2, window.devicePixelRatio)
    this.renderer.setPixelRatio(dpr)

    // set camera position
    this.moveCameraVec = new THREE.Vector3(0, 0, 1)
    this.camera.lookAt( 0, 0, -2 * this.maxZPosition)
  },
  mounted() {
    this.$refs.container.appendChild(this.renderer.domElement)

    // Setup our scene meshes
    this.createOrbs()

    // Initiate the animation loop, call draw on every "tick"
    this.loop = rafLoop(this.draw)
    this.clock = new THREE.Clock()
  },
  destroyed() {
    this.loop.stop().removeAllListeners()
    this.renderer.dispose()
  },
  watch: {
    width() {
      this.handleWindowResize()
    },
    height() {
      this.handleWindowResize()
    },
  },
  methods: {
    draw() {
      const time = this.clock.getElapsedTime()
      this.renderer.render(this.scene, this.camera)
    },
    createOrbs() {
    },
    handleWindowResize() {
      this.renderer.setSize(this.width, this.height)
      this.camera.aspect = this.width / this.height
      this.camera.updateProjectionMatrix()
      this.draw()
    },
  }
}
</script>

<style scoped>
#world {
  position: absolute;
  top: 0;
  left: 0;
}

.hidden {
  position: absolute;
  top: 0;
  left: 0;
  display: none;
}
</style>
