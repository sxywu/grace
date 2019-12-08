const fs = require('fs')
const _ = require('lodash')
const wiki = require('wikijs').default

const namesFile = '20names'
const detailsFile = '20details'

// all names already has name, url, year, fields, shortSummary, summary,
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
        page.summary(),
      ]).then(([backlinks, summary]) => {
        details.push(Object.assign({}, allNames[index], {
          backlinks: backlinks.length,
          summary,
        }))

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

getDetails(_.difference(_.map(allNames, 'name'), existingNames))
