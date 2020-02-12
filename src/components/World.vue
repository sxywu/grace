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

const navy = 0x0B1E38
const purple = 0x362C51
const blue = 0x51629E
const yellow = 0xFFE78B
const pink = 0xDD867E
const beige = 0xEBBA8B

export default {
  name: 'world',
  props: ['width', 'height', 'maxZPosition', 'orbs', 'tl'],
  data() {
    return {
      opacity: 1,
      display: 'block',
      textWidth: 820,
      textHeight: 420,
      decades: _.range(8),
      cameraXPosition: 0,
      cameraZPosition: 5,
      // cameraPosition: {x: 0, y: 0, z: 5},
    }
  },
  created() {
    this.scene = new THREE.Scene()
    this.camera = new THREE.PerspectiveCamera(45, this.width / this.height, 1, 1000)
    this.renderer = new THREE.WebGLRenderer({antialias: true, alpha: true})

    // WebGL background color
    this.renderer.setClearColor(0xffffff, 0)

    // lights
    // const hemisphere1 = new THREE.HemisphereLight( purple, blue, 1 )
    // this.scene.add(hemisphere1)
    const hemisphere2 = new THREE.HemisphereLight( pink, navy, 1.5 )
    this.scene.add(hemisphere2)
    const ambient = new THREE.AmbientLight(beige , 1.2 )
    this.scene.add(ambient)
    this.light = new THREE.DirectionalLight( pink, 1.5 )
    this.light.position.set(0, 100, 100)
    this.scene.add( this.light )

    // set renderer size & pixel ratio
    this.renderer.setSize(this.width, this.height)
    const dpr = Math.min(2, window.devicePixelRatio)
    this.renderer.setPixelRatio(dpr)

    // set camera position
    this.moveCameraVec = new THREE.Vector3(0, 0, 1)
    this.camera.lookAt( 0, 0, -2 * this.maxZPosition)
    this.createTimeline()
  },
  mounted() {
    this.$refs.container.appendChild(this.renderer.domElement)

    // Initiate the animation loop, call draw on every "tick"
    this.loop = rafLoop(this.draw)
    this.clock = new THREE.Clock()

    this.createOrbs()
    this.createBackground()
    this.draw()
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
    orbs() {
      this.createTimeline()
      this.createOrbs()
      this.draw()
    },
    cameraZPosition() {

      // update camera position
      this.camera.position.set(
        this.cameraXPosition,
        0,
        this.cameraZPosition,
      )

      // this.camera.position.addScaledVector(this.moveCameraVec, 1)
      this.camera.updateMatrixWorld()
      this.renderer.render(this.scene, this.camera)
    },
  },
  methods: {
    draw() {
      const time = this.clock.getElapsedTime()
      this.renderer.render(this.scene, this.camera)
    },
    createTimeline() {
      // timeline
      _.each(this.orbs, ({x, z}, i) => {
        this.tl.to(this.$data, {
          cameraXPosition: x,
          cameraZPosition: z + 5,
        }, i)
      })
    },
    createOrbs() {
      const starGeometry = new THREE.SphereGeometry(0.1, 20, 20)
      _.each(this.orbs, ({x, y, z}) => {
        const starMaterial = new THREE.MeshBasicMaterial( {
          color: yellow,
          side: THREE.DoubleSide,
        })
        const mesh = new THREE.Mesh(starGeometry, starMaterial)
        mesh.position.set(x, y, z)
        this.scene.add( mesh )
      })
    },
    createBackground() {
      // textured floor inspiration from
      // https://tympanus.net/codrops/2016/04/26/the-aviator-animating-basic-3d-scene-threejs/
      const planeSize = 2 * this.maxZPosition
      const plane = new THREE.Mesh(
        new THREE.PlaneGeometry(planeSize, planeSize, planeSize, planeSize / 2),
        new THREE.MeshStandardMaterial( {
          color: navy,
          side: THREE.DoubleSide,
      		flatShading: true,
          roughness: 1.0,
        } )
      )
      _.each(plane.geometry.vertices, v => {
        v.x += _.random(-0.25, 0.25)
        v.y += _.random(-0.1, 0.1)
        v.z += _.random(-0.25, 0.25)
      })
      plane.rotateX(-Math.PI / 2)
      plane.translateZ(-3)
      this.scene.add( plane )

      // and add "sky"
      const sky = new THREE.Mesh(
        new THREE.SphereGeometry(this.maxZPosition / 2 + 10, 20, 20),
        new THREE.MeshStandardMaterial( {
          color: navy,
          side: THREE.BackSide,
        } )
      )
      this.scene.add( sky )
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
