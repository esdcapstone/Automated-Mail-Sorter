



import React from 'react'
import './App.css'

import 'fontsource-roboto'
import { Grid } from '@mui/material'
import { ThemeProvider, createTheme } from '@mui/material/styles'
import { responsiveFontSizes } from '@mui/material'
import { CssBaseline } from '@mui/material'
import Button from '@mui/material/Button'
import { Typography } from '@mui/material'
const defaultTheme = createTheme({
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
})

const {breakpoints, typography: { pxToRem } } = defaultTheme
let theme = {...defaultTheme,
  typography:{
    h1:{fontSize: '1.9rem',
  '@media (min-width:600px)': {
    fontSize: '2.5rem',
  },
  [defaultTheme.breakpoints.up('md')]: {
    fontSize: '3.4rem',
  },
}}}


function App() {
  return (
    <ThemeProvider theme={theme}>
      <Typography variant='h1'>Automated Mail Sorter</Typography>
      <Button variant="contained" color="primary">Hello world</Button>
    </ThemeProvider>

  )
}

export default App;



