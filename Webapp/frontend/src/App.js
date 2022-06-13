



import React from 'react'
import './App.css'

import 'fontsource-roboto'
import {  Grid } from '@mui/material'
import { ThemeProvider, createTheme } from '@mui/material/styles'
import { responsiveFontSizes } from '@mui/material'
import { CssBaseline } from '@mui/material'
import Button from '@mui/material/Button'
import { Typography } from '@mui/material'
let theme = createTheme({
  palette: {
    primary: {
      main: '#091425',
      contrastText: '#E2DEDE'



    },
    secondary: {
      
      main: '#e0164a',
      contrastText: '#E2DEDE'
    }
  },
  
  typography:{

  }
})

theme = responsiveFontSizes(theme);

function App() {
  return (
    <ThemeProvider theme={theme}>
      <Typography variant='h1'>Automated Mail Sorter</Typography>
      <Button variant= "contained" color = "primary">Hello world</Button>
    </ThemeProvider>

  )
}

export default App;



