const fs = require('fs')
const https = require('https')
const _ = require('lodash')
const wiki = require('wikijs').default

const credentials = fs.readFileSync('./data/key.txt', {encoding: 'utf8'})

// get all links in computing page
wiki().page('Women in computing')
  .then(page => {
    Promise.all([
        page.links(),
      ]).then(([links]) => {
        links = _.filter(links, link => link[0] !== '.')
        const name = links.shift()
        isWoman(name, links)
      }).catch(err => {
        console.log('get page links ERROR: ', err)
      })
  }).catch(err => {
    console.log('get page ERROR: ', err)
  })

const names = []
// Go through each link in the page and ask Google Knowledge Graph
// if it's a person, and see if they're a woman
function isWoman(name, links) {
  let url = `https://kgsearch.googleapis.com/v1/entities:search?`
  url += `query=${name}&key=${credentials}&limit=1&indent=True`

  https.get(url, res => {
    let body = ''
    res.on('data', chunk => {
      body += chunk
    })

    res.on('end', () => {
      try {
        body = JSON.parse(body)

        const type = body.itemListElement && body.itemListElement[0] &&
          body.itemListElement[0].result && body.itemListElement[0].result["@type"] &&
          body.itemListElement[0].result["@type"][0]
        if (!type || type !== 'Person') {
          goToNext(name, links)
          return
        }

        // if they're a person, see if they're a woman
        const {url, articleBody} = body.itemListElement[0].result.detailedDescription
        const femaleMatches = articleBody.match(/\s(she|her)[\s\.\,]/gmi)
        const maleMatches = articleBody.match(/\s(he|him)[\s\.\,]/gmi)

        console.log(`\nquerying for ${name}`)
        if ((femaleMatches && !maleMatches) ||
          (femaleMatches && maleMatches && femaleMatches.length > maleMatches.length)) {
          // if there are she/her matches but no he/him matches
          // or there are matches for both but there are more she/her matches
          names.push({name, url, summary: articleBody, isCertain: true})
        } else if (!femaleMatches && !maleMatches) {
          // if there were no matches, manually check
          names.push({name, url, summary: articleBody, isCertain: false})
        } else {
          // if there are more he/him matches, log it and move on
          console.log('!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ', name, femaleMatches, maleMatches)
          goToNext(name, links)
          return
        }

        console.log(`writing ${name}`)
        fs.writeFileSync(`./data/names.json`, JSON.stringify(names), {encoding: 'utf8'})
        goToNext(name, links)
      } catch(err) {
        console.error('JSON parse ERROR: ', err)
        goToNext(name, links)
      }
    })
  }).on('error', (e) => {
    console.error('get google knowledge ERROR: ', err)
    goToNext(name, links)
  });
}

function goToNext(name, links) {
  if (links.length) {
    name = links.shift()
    isWoman(name, links)
  }
}
