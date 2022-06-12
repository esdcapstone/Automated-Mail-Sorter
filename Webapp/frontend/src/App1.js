import React from 'react'
import './App.css'
import Button from '@mui/material/Button'
import ButtonGroup from '@mui/material/ButtonGroup'
import SaveIcon from '@mui/icons-material/Save'
import DeleteIcon from '@mui/icons-material/Delete'
import Checkbox from '@mui/material/Checkbox'
import FormControlLabel from '@mui/material/FormControlLabel'
import TextField  from '@mui/material/TextField'
import {makeStyles, ThemeProvider} from '@material-ui/styles'
import {createTheme} from '@mui/material/styles'
import {green, orange} from '@mui/material/colors'
import 'fontsource-roboto'
import { Typography } from '@mui/material'
import { Container } from '@mui/system'
import { Paper } from '@mui/material'
import { Grid } from '@mui/material'

const useStyles = makeStyles({
  root: {
    background: 'linear-gradient(75deg, #FE6B8B, #FF8E53)',
    color: "secondary",
    border: 0,
    borderRadius: 40,
    padding: '0 30px',
    marginButtom: 15
  }
})

const theme = createTheme({
  palette:{
    primary:{
      main: orange[500]
    },
    secondary: {
      main: green[500]
    }
  }
})

function ButtonStyled(){
  const classes = useStyles();
  return <Button className = {classes.root} color = "secondary">Test Styled Button</Button>
}

function CheckBoxExample(){
  const [checked, setChecked] = React.useState(true)
  return (
    <FormControlLabel 
    control = {<Checkbox 
      checked = {checked}
      icon= {<DeleteIcon/>}
      checkedIcon= {<SaveIcon/>}
       onChange = {(e)=>setChecked(e.target.checked)}
    />}
    label = "Testing Checkbox"
    />
    
  )
}



function App() {
  return (
    <ThemeProvider theme = {theme}>
      <Container maxWidth = "md"> 
        <div className='App'>
          <header className = "App-header">
            <Typography variant= "h2">
              Welcome to MUI  
            </Typography>
            <ButtonStyled/>
            <Grid container spacing = {2} justify= "center">
            <Grid item >
                <Paper style={{height:75, width: 100, backgroundColor: 'white' }}/>
              </Grid><Grid item>
                <Paper style={{height:75, width: 50, backgroundColor: 'white' }}/>
              </Grid><Grid item>
                <Paper style={{height:75, width: 50, backgroundColor: 'white' }}/>
              </Grid>
            </Grid>
                


            <TextField
            variant = "outlined"
            color = "secondary"
            type = "email"
            label = "The Time"
            placeholder = "test@test.com"/>
            <CheckBoxExample />
            <ButtonGroup variant="contained">
            <Button startIcon = {<SaveIcon />} onClick = {()=>alert('hello')} size = "small" href = "#"  >
            Hello World
            </Button>
            <Button startIcon = {<DeleteIcon />} onClick = {()=>alert('hello')} size = "small" href = "#" >
            Discard
            </Button>
            </ButtonGroup>
          </header>
        </div>
      </Container>
    </ThemeProvider>
    
    
  );
}

export default App;

