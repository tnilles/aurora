const panel = document.getElementById('panel')
const DIMENSIONS = {X: 27, Y: 13}
const MODE = 'blurry'

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
    if (MODE == 'blurry') return document.querySelector(`.led-${i}`).style.boxShadow = `0px 0px 24px 21px ${color}`
    document.querySelector(`.led-${i}`).style.background = color
    document.querySelector(`.led-${i}`).style.width = '20px'
    document.querySelector(`.led-${i}`).style.height = '20px'
    document.querySelector(`.led-${i}`).style.borderRadius = '0'
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
      strip.setPixelColor(i, `hsla(${pixelHue}, 100%, 50%, 0.3)`)
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
        strip.setPixelColor(c, `hsla(${hue}, 100%, 50%, 0.3)`)
      }
      await delay(wait)
      firstPixelHue += 360 / 90 // One cycle of color wheel over 90 frames
    }
  }
}

const usFlagMatrix = () => {
  let stripMatrix = []

  // Stripes
  for (let row = 0; row < DIMENSIONS.Y; row++) {
    stripMatrix.push([])
    let color = row % 2 == 0 ? 'rgba(180, 47, 53, 0.3)' : 'rgba(255, 255, 255, 0.3)'
    for (let col = 0; col < DIMENSIONS.X; col++) {
      stripMatrix[row].push(color)
    }
  }

  // and Stars
  for (let row = 0; row < DIMENSIONS.Y; row++) {
    for (let col = 0; col < DIMENSIONS.X; col++) {
      if (col < 10 && row < 7) {
        if (col > 0 && col < 9 && row > 0 && row < 6 && ((row * 9 + col) % 2 == 0)) {
          stripMatrix[row][col] = ('rgba(255, 255, 255, 0.3)')
        } else {
          stripMatrix[row][col] = ('rgba(59, 59, 111, 0.3)')
        }
      }
    }
  }

  return stripMatrix
}

const usFlag = async (wait) => {
  let shift = 0
  for (let i = 0; i < 100; i++) {
    renderStripMatrix(applyDisplacementMap(wavyDisplacementMap(shift), usFlagMatrix(), 'up'))
    shift += 2
    await delay(wait)
    renderStripMatrix(usFlagMatrix())
    await delay(wait)
    renderStripMatrix(applyDisplacementMap(wavyDisplacementMap(shift), usFlagMatrix(), 'down'))
    shift += 2
    await delay(wait)
    renderStripMatrix(usFlagMatrix())
    await delay(wait)
  }
}

const renderStripMatrix = (stripMatrix) => {
  strip.clear()
  for (let row = 0; row < DIMENSIONS.Y; row++) {
    for (let col = 0; col < DIMENSIONS.X; col++) {
      strip.setPixelColor(cartesianToAlternatedStrip({x: col, y: row}), stripMatrix[row][col])
    }
  }
}

// Displacement maps

// 0 0 1 1 0 0 1 1 0 0 … 
// 0 0 1 1 0 0 1 1 0 0 …
// 0 0 1 1 0 0 1 1 0 0 …
const wavyDisplacementMap = (shift = 0) => {
  const colValue = col => {
    let shiftedCol = col - shift
    let x = shiftedCol == 0 ? 0 : shiftedCol / 6
    return (Math.floor(x) % 2 == 0) ? 0 : 1
  }

  let map = []
  for (let row = 0; row < DIMENSIONS.Y; row++) {
    map.push([])

    for (let col = 0; col < DIMENSIONS.X; col++) {
      map[row].push(colValue(col))
    }
  }
  return map
}

const deepCopy = x => JSON.parse(JSON.stringify(x))

// map: [[0, 1, 0, 1,…], […], …]
// stripMatrix: [['rgba(255, 255, 255, 0.3)', 'rgba(255, 255, 255, 0.3)', …], […], …]
// returns an updated stripMatrix
const applyDisplacementMap = (map, stripMatrix, direction = 'up') => {
  let displacedMatrix = deepCopy(stripMatrix)
  const rowInit = (direction == 'down') ? DIMENSIONS.Y - 1 : 0
  const rowCond = row => (direction == 'down') ? row >= 0 : row < DIMENSIONS.Y
  const rowIter = row => (direction == 'down') ? row - 1 : row + 1
  const rowToChange = row => (direction == 'down') ? row + 1 : row - 1

  for (let row = rowInit; rowCond(row); row = rowIter(row)) {
    for (let col = 0; col < DIMENSIONS.X; col++) {
      let originalPx = displacedMatrix[row][col]
      let originalPxArray = originalPx.split(',')

      if (map[row][col] == 1 && rowToChange(row) >= 0 && rowToChange(row) < DIMENSIONS.Y) {
        originalPxArray[originalPxArray.length - 1] = '0.3'
        displacedMatrix[rowToChange(row)][col] = originalPxArray.join(',')
      }
      
      if (map[row][col] == 1 && row == 0 && direction == 'down') {
        originalPxArray[originalPxArray.length - 1] = '0.1'
        displacedMatrix[0][col] = originalPxArray.join(',') //'rgba(0, 0, 0, 1)'
      }
      
      if (map[row][col] == 1 && row == DIMENSIONS.Y - 1 && direction == 'up') {
        originalPxArray[originalPxArray.length - 1] = '0.1'
        displacedMatrix[DIMENSIONS.Y - 1][col] = originalPxArray.join(',') //'rgba(0, 0, 0, 1)'
      }
    }
  }

  return displacedMatrix
}

const regularStripToCartesian = (i) => {
  const y = Math.floor(i / DIMENSIONS.X)
  const x = i - DIMENSIONS.X * y
  return {x, y}
}

const alternatedStripToCartesian = (i) => {
  const y = Math.floor(i / DIMENSIONS.X)
  const x = (y % 2 == 0) ? i - DIMENSIONS.X * y : (DIMENSIONS.X - 1) - (i - DIMENSIONS.X * y)
  return {x, y}
}

const cartesianToRegularStrip = ({x, y}) => {
  return DIMENSIONS.X * y + x
}

const cartesianToAlternatedStrip = ({x, y}) => {
  return DIMENSIONS.X * y + (y % 2 == 0 ? x : (DIMENSIONS.X - 1) - x)
}

const regularToAlternated = i => {
  return cartesianToAlternatedStrip(regularStripToCartesian(i))
}

(async () => {
  await usFlag(100)
  await colorWipe('rgb(255, 0, 0, 0.3)', 50)
  await colorWipe('rgb(0, 255, 0, 0.3)', 50)
  await colorWipe('rgb(0, 0, 255, 0.3)', 50)
  await theaterChase('rgb(200, 200, 200, 0.3)', 50)
  await theaterChase('rgb(255, 0, 0, 0.3)', 50)
  await theaterChase('rgb(0, 0, 255, 0.3)', 50)
  await rainbow(10)
  await theaterChaseRainbow(50)
})()