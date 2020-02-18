<template>
  <div id='world' :style='{display, opacity}'>
    <canvas class='hidden' v-for='i in 18' :ref='`orb${i - 1}`'
      :width='2 * canvasWidth' :height='2 * canvasHeight'
      :style='{width: `${canvasWidth}px`, height: `${canvasHeight}px`}'></canvas>
    <div ref='container' :width='width' :height='height'></div>
  </div>
</template>

<script>
import _ from 'lodash'
import rafLoop from 'raf-loop'
import * as d3 from 'd3'
import * as THREE from 'three'
import p5 from 'p5'

const navy = 0x0B1E38
const purple = 0x362C51
const blue = 0x51629E
const yellow = 0xFFE78B
const pink = 0xDD867E
const beige = 0xEBBA8B

const colorInterpolate = d3.interpolateCubehelix('#FFE78B', '#DD867E') // yellow, pink
const canvasWidth = 256
const canvasHeight = 256
const circleRadius = 80
const circleOpacities = [0.05, 0.1, 1]
const circleRadii = [1, 0.75, 0.5]
const fadeThreshold = 15
let xoff = 0

export default {
  name: 'world',
  props: ['width', 'height', 'maxZPosition', 'orbs', 'tl', 'tl0'],
  data() {
    return {
      display: 'none',
      opacity: 0,
      canvasWidth,
      canvasHeight,
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
    this.camera.position.set( 0, 0, 10 )
    this.camera.lookAt( 0, 0, -2 * this.maxZPosition)
  },
  mounted() {
    this.$refs.container.appendChild(this.renderer.domElement)

    // Initiate the animation loop, call draw on every "tick"
    this.loop = rafLoop(this.draw)
    this.clock = new THREE.Clock()

    this.storeOrbCanvases()
    this.calculateCircles()
    this.createOrbs()
    this.createTimeline()
    this.createBackground()
    this.createStars()
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
      this.storeOrbCanvases()
      this.calculateCircles()
      this.createOrbs()
      this.createTimeline()
      this.draw()
    },
  },
  methods: {
    draw() {
      if (!this.orbs.length) return

      const elapsed = this.clock.getElapsedTime()
      this.drawCircles(elapsed)

      const cameraZPosition = this.camera.position.z
      _.each(this.orbs, ({mesh, z}) => {
        // update texture so that orbs will animate!
        mesh.material.map.needsUpdate = true

        const dist = Math.abs(z - cameraZPosition)
        const scale = 1 - (dist > fadeThreshold ? (dist - fadeThreshold) / (1.5 * fadeThreshold) : 0)
        // mesh.material.opacity = scale
        mesh.scale.set(scale, scale, scale)
      })

      this.renderer.render(this.scene, this.camera)
    },
    calculateCircles() {
      if (!this.orbs.length) return

      const numScale = d3.scaleQuantize()
        .domain(d3.extent(this.orbs, d => d.references))
        .range(_.range(4, 8))
      const speedScale = d3.scaleLog()
        .domain(d3.extent(this.orbs, d => d.birthYear ? d.year - d.birthYear : 36))
        .range([0.5, 1])

      _.each(this.orbs, (d, i) => {
        const numCircles = numScale(d.references)
        const speed = speedScale(d.birthYear ? d.year - d.birthYear : 36)
        d.circles = _.times(numCircles, j => {
          return {
            cx: canvasWidth / 2,
            cy: canvasHeight / 2,
            radius: p5.prototype.randomGaussian(circleRadius, 10),
            color: colorInterpolate(p5.prototype.randomGaussian(0.25, 0.15)),
            offset: i * 1000 + j * 10,
            numCircles, speed,
          }
        })
      })
    },
    storeOrbCanvases() {
      if (!this.orbs.length) return

      _.each(this.orbs, (d, i) => {
        const canvas = this.$refs[`orb${i}`][0]
        const ctx = canvas.getContext('2d')
        ctx.scale(2, 2)
        ctx.globalCompositeOperation = 'screen'

        Object.assign(d, {canvas, ctx})
      })
    },
    drawCircles(elapsed) {
      xoff += 0.01

      _.each(this.orbs, ({circles, ctx}) => {
        // first clear canvas
        ctx.clearRect(0, 0, canvasWidth, canvasHeight)

        _.each(circles, ({cx, cy, radius, color, offset, numCircles, speed}, i) => {
          // first, calculate x and y positions
          const noise = p5.prototype.noise(offset + xoff)
          const t = i * elapsed / (numCircles * speed) + offset
          let x = 0.65 * noise * radius * Math.cos(t) * Math.sin(t)
          let y = 0.5 * noise * radius * Math.sin(t)
          x *= (i % 2) ? Math.cos(t) : Math.sin(t)

          // add to central x/y
          x += cx + 10 * noise
          y += cy + 10 * noise

          _.times(3, i => {
            ctx.globalAlpha = circleOpacities[i]
            ctx.fillStyle = color
            ctx.beginPath()
            ctx.arc(x, y, circleRadii[i] * radius, 0, 2 * Math.PI, 0)
            ctx.fill()
          })
        })
      })
    },
    createOrbs() {
      if (!this.orbs.length) return

      const geometry = new THREE.PlaneGeometry(1.25, 1.25, 1, 1)
      _.each(this.orbs, (d) => {
        const {x, y, z, canvas} = d
        const texture = new THREE.Texture(canvas)

        const material = new THREE.MeshBasicMaterial({
          map: texture,
          transparent: true,
          opacity: 1.0,
        })
        material.map.needsUpdate = true

        const mesh = new THREE.Mesh(geometry, material)
        mesh.position.set(x, y, z)
        this.scene.add( mesh )

        d.mesh = mesh
      })
    },
    createTimeline() {
      if (!this.orbs.length) return
      
      // fade the world in
      this.tl0.set(this.$data, {display: 'block'}, 2.5)
      this.tl0.to(this.$data, {opacity: 1, duration: 1.5}, 2.5)
      this.tl0.add(() => this.loop.stop(), 3)
      this.tl0.add(() => this.loop.start(), 3.01)

      // timeline
      _.each(this.orbs, ({mesh, x, y, z}, i) => {
        this.tl.to(this.camera.position, {x, y, z: z + 2}, i)
      })
      this.tl.to(this.camera.position, {
        x: 0, y: 0, z: -this.maxZPosition - 5, duration: 2,
      }, this.orbs.length)
    },
    createBackground() {
      // textured floor inspiration from
      // https://tympanus.net/codrops/2016/04/26/the-aviator-animating-basic-3d-scene-threejs/
      const planeSize = this.maxZPosition + 20
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
        new THREE.SphereGeometry(planeSize, 20, 20),
        new THREE.MeshStandardMaterial( {
          color: navy,
          side: THREE.BackSide,
        } )
      )
      sky.translateZ(-3)
      this.scene.add( sky )
    },
    createStars() {
      const starGeometry = new THREE.CircleGeometry(0.075, 20, 20)
      const starMaterial = new THREE.MeshBasicMaterial( {
        color: 0xfffef5,
      })
      _.times(480, i => {
        // small white dot
        const star = new THREE.Mesh(starGeometry, starMaterial)
        const angle = p5.prototype.randomGaussian(-Math.PI / 2, Math.PI / 2)
        const radius = this.maxZPosition + _.random(20)
        const scale = Math.random()
        star.position.set(
          radius * Math.cos(angle), // x
          _.random(-3, this.maxZPosition / 2), // y
          radius * Math.sin(angle), // z
        )
        star.scale.set(scale, scale, scale)
        this.scene.add( star )
      })
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
