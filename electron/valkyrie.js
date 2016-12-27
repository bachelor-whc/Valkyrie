const valkyrie_addon = require('./build/Release/valkyrie');
//const valkyrie_addon = require('./build/Release/test');
const {app, BrowserWindow} = require('electron')
const path = require('path')
const url = require('url')

let win

function createWindow() {
  win = new BrowserWindow({ width: 800, height: 600 , frame: false})

  console.log(win.getNativeWindowHandle())
  valkyrie_addon.InitializeValkyrie(800, 600, win.getNativeWindowHandle())

  win.on('closed', () => {
    win = null
  })
}

app.on('ready', createWindow)

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})

app.on('activate', () => {
  if (win === null) {
    createWindow()
  }
})