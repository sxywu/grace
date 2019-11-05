const fs = require('fs')
const _ = require('lodash')
const wiki = require('wikijs').default

const namesFile = '29names'
const detailsFile = '29details'

const allNames = JSON.parse(fs.readFileSync(`./data/${namesFile}.json`, {encoding: 'utf8'}))
const details = fs.existsSync(`./data/${detailsFile}.json`) ?
  JSON.parse(fs.readFileSync(`./data/${detailsFile}.json`, {encoding: 'utf8'})) : []
const existingNames = _.map(details, 'name')
console.log(allNames.length, existingNames.length)

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
        details.push({
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
        fs.writeFileSync(`./data/${detailsFile}.json`, JSON.stringify(details), {encoding: 'utf8'})
        if (index < names.length) {
          index += 1
          getDetails(names)
        }
      }).catch(err => {
        console.log('get page details error: ', err)
        index += 1
        getDetails(names)
      })
    }).catch(err => {
      console.log('get page error: ', err)
      index += 1
      getDetails(names)
    })
}

getDetails(_.difference(allNames, existingNames))
