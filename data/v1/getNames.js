const fs = require('fs')
const _ = require('lodash')
const https = require('https')
const {JSDOM} = require('jsdom')

// first get every page from category Women_computer_scientists
// wiki().pagesInCategory('Agriculture')
//   .then(pages => console.log(pages.length))

const urls = [
  'https://en.wikipedia.org/wiki/Category:Women_computer_scientists',
  'https://en.wikipedia.org/w/index.php?title=Category:Women_computer_scientists&pagefrom=Joseph%2C+Deborah%0ADeborah+Joseph#mw-pages',
  'https://en.wikipedia.org/w/index.php?title=Category:Women_computer_scientists&pagefrom=Windsor%2C+Edith%0AEdith+Windsor#mw-pages',
]

const allNames = []
function getPages(url) {
  https.get(url, res => {
    let html = ''
    res.on('data', chunk => {
      html += chunk
    })

    res.on('end', () => {
      // const matches = data.match(/href="\/wiki\/(.*)" /gm)
      const {document} = (new JSDOM(html)).window
      _.each(document.querySelectorAll('#mw-pages .mw-category-group a'), anchor => {
        allNames.push(anchor.textContent)
        console.log(anchor.textContent)
      })

      if (urls.length) {
        // if there's still urls left, then get that category page
        getPages(urls.shift())
      } else {
        fs.writeFileSync('./data/names.json', JSON.stringify(allNames), {encoding: 'utf8'})
      }
    })
  })
}

getPages(urls.shift())
