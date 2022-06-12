



import React from 'react'
import './App.css'
// import Box from '@mui/material/Box'
import { Button, Grid} from '@mui/material'


function App() {
  return (
    <Grid container id='BaseContainer' gridAutoRows="true">
      <Grid container item xs={12}  sx={{ backgroundColor: "rgb(197, 194, 194)", 
      alignItems: 'center', justifyContent: "center"}} >
        <Grid item xs = {2} sx = {{ textAlign: 'center', backgroundColor: "rgb(65, 62, 100)" }}><Button>Welcome</Button></Grid>
        <Grid item xs={8} sx = {{ textAlign: 'center' ,backgroundColor: 'rgb(66, 100, 64)'}}>Title</Grid>
        <Grid item  xs = {2} sx = {{ textAlign: 'center',backgroundColor: 'rgb(100, 62, 62)'}}>Logout</Grid>
      </Grid>

      <Grid item xs={12} backgroundColor = "violet" textAlign="center">
        Big Container
      </Grid>
      <Grid item xs={12}>
        Three small containers
      </Grid>
      <Grid item xs={12}>
        Footer
      </Grid>
    </Grid>
  )
}

export default App;



