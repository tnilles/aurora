const panel = document.getElementById('panel')
const DIMENSIONS = {X: 28, Y: 13}

const buildSnakePanel = () => {
  panel.innerHTML = ''
  for (let row = 0; row < DIMENSIONS.Y; row++) {
    let rowContainer = document.createElement('div')
    rowContainer.classList.add('row')
    panel.append(rowContainer)

    for (let col = 0; col < DIMENSIONS.X; col++) {
      let led = document.createElement('div')
      led.classList.add('led')
      let ledLight = document.createElement('div')
      ledLight.classList.add('led-light')
      ledLight.classList.add(`led-${(row * DIMENSIONS.X) + col}`)
      led.append(ledLight)

      if (row % 2 == 0) {
        rowContainer.append(led)
      } else {
        rowContainer.prepend(led)
      }
    }
  }
}

buildSnakePanel()

const delay = (wait) => new Promise(resolve => setTimeout(() => resolve(), wait))

// Simulate Adafruit_NeoPixel API
const strip = {
  show: () => {
    [...document.querySelectorAll('.led-light')].forEach(l => {
      l.style.boxShadow = '0px 0px 24px 21px transparent'
    })
  },

  setPixelColor: (i, color) => {
    document.querySelector(`.led-${i}`).style.boxShadow = `0px 0px 24px 21px ${color}`
  },

  numPixels: () => [...document.querySelectorAll('.led-light')].length,

  clear: () => {
    [...document.querySelectorAll('.led-light')].forEach(l => {
      l.style.boxShadow = '0px 0px 24px 21px transparent'
    })
  }
}

// Replicate Adafruit_NeoPixel example code
const colorWipe = async (color, wait) => {
  for (let i = 0; i < strip.numPixels(); i++) {
    await delay(wait)
    strip.setPixelColor(i, color)
  }
}

const theaterChase = async (color, wait) => {
  for(let a = 0; a < 10; a++) {
    for(let b = 0; b < 3; b++) {
      strip.clear()
      for(let c = b; c < strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color)
      }
      await delay(wait)
    }
  }
}

const rainbow = async (wait) => {
  for(let firstPixelHue = 0; firstPixelHue < 360; firstPixelHue += 2.40625) {
    for(let i = 0; i < strip.numPixels(); i++) {
      let pixelHue = firstPixelHue + (i * 360 / strip.numPixels())
      strip.setPixelColor(i, `hsla(${pixelHue}, 100%, 50%, 0.8)`)
    }
    await delay(wait)
  }
}

const theaterChaseRainbow = async (wait) => {
  let firstPixelHue = 0
  for(let a = 0; a < 30; a++) {
    for(let b = 0; b < 3; b++) {
      strip.clear()

      for(let c = b; c < strip.numPixels(); c += 3) {
        let hue   = firstPixelHue + c * 360 / strip.numPixels()
        strip.setPixelColor(c, `hsla(${hue}, 100%, 50%, 0.8)`)
      }
      await delay(wait)
      firstPixelHue += 360 / 90 // One cycle of color wheel over 90 frames
    }
  }
}

(async () => {
  await colorWipe('rgb(255, 0, 0, 0.8)', 50)
  await colorWipe('rgb(0, 255, 0, 0.8)', 50)
  await colorWipe('rgb(0, 0, 255, 0.8)', 50)
  await theaterChase('rgb(200, 200, 200, 0.8)', 50)
  await theaterChase('rgb(255, 0, 0, 0.8)', 50)
  await theaterChase('rgb(0, 0, 255, 0.8)', 50)
  await rainbow(10)
  await theaterChaseRainbow(50)
})()