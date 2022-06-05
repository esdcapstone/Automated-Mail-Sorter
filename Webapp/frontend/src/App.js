import React from 'react'
import './App.css'
import Box from '@mui/material/Box'
import {Grid} from '@mui/material'
function App(){
  return(
    <Grid id='BaseContainer'>
      <Box 
        sx = {{
          width: '40%',
          height: '100%',
          backgroundColor: 'pink',
        }}
      /><Box 
      sx = {{
        width: '40%',
        height: '100%',
        backgroundColor: 'pink',
      }}
    />
    </Grid>
  )
}

export default App;