const wiki = require('wikijs').default
const fs = require('fs')
const _ = require('lodash')
const https = require('https')
const {JSDOM} = require('jsdom')

// first get every page from category Women_computer_scientists
// wiki().pagesInCategory('Agriculture')
//   .then(pages => console.log(pages.length))

const urls = [
  'https://en.wikipedia.org/wiki/Category:Women_computer_scientists',
  'https://en.wikipedia.org/w/index.php?title=Category:Women_computer_scientists&pagefrom=Joseph%2C+Deborah%0ADeborah+Joseph#mw-pages'
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
        getDetails(allNames)
      }
    })
  })
}

const data = []
let index = 0
function getDetails(names) {
  const name = names[index]
  wiki().page(name)
    .then(page => {
      Promise.all([
        page.backlinks(),
        page.references(),
        page.fullInfo(),
        page.links(),
        page.summary(),
        page.mainImage(),
      ]).then(([backlinks, references, info, links, summary, mainImage]) => {
        data.push({
          url: page.fullurl,
          name,
          backlinks,
          references,
          info,
          links,
          summary,
          mainImage,
        })

        console.log(`writing ${name}`)
        fs.writeFileSync('./data/data.json', JSON.stringify(data), {encoding: 'utf8'})
        if (index < names.length) {
          index += 1
          getDetails(names)
        }
      })
    })
}

getPages(urls.shift())
