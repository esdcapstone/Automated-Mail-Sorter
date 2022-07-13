import React from "react";
import { useState } from "react";
import "./App.css";

import "fontsource-roboto";
import { Grid } from "@mui/material";
import { ThemeProvider, createTheme, styled } from "@mui/material/styles";
import { responsiveFontSizes } from "@mui/material";
import { CssBaseline } from "@mui/material";
import Button from "@mui/material/Button";
import { Typography } from "@mui/material";
import { Box } from "@mui/system";
import { Paper } from "@mui/material";

// Create a basic theme
const defaultTheme = createTheme({
  palette: {
    primary: {
      main: "#091425",
      contrastText: "#dcd3d3",
    },
    secondary: {
      main: "#dc1f04",
      contrastText: "#E2DEDE",
    },
  },
});

const StyledPaper = styled("Paper")(({ theme }) => ({
  backgroundColor: theme.palette.primary.main,
  color: theme.palette.primary.contrastText,
}));
// Make responsive text size
// const { breakpoints, typography: { pxToRem } } = defaultTheme
let theme = {
  ...defaultTheme,
  typography: {
    h1: {
      fontSize: "1.5rem",
      "@media (min-width:600px)": {
        fontSize: "2.4rem",
      },
      [defaultTheme.breakpoints.up("sm")]: {
        fontSize: "3rem",
      },
    },
  },
};

function App() {
  const [val1, setVal1] = useState(0);
  const [val2, setVal2] = useState(0);
  const [val3, setVal3] = useState(0);

  function f1() {
    setVal1(val1 + 1);
  }
  function f2() {
    setVal2(val2 + 1);
  }
  function f3() {
    setVal3(val3 + 1);
  }
  return (
    <ThemeProvider theme={theme}>
      <Grid container justifyContent="center">
        <Typography variant="h1" color="primary">
          Automated Mail Sorting
        </Typography>
      </Grid>
      <Grid container justifyContent="center">
        <Button
          variant="contained"
          sx={{
            height: "5rem",
            margin: "2rem",
          }}
        >
          Start Sorting
        </Button>
        {/* <StyledPaper
          elevation={10}
          sx={{
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
            height: "6rem",
            width: "6rem",
            margin: "2rem",
          }}
        >
          <Typography variant="h6">Start Sorting</Typography>
        </StyledPaper> */}
      </Grid>
      <Grid container padding="25px" justifyContent="space-around">
        <Grid item>
          <StyledPaper
            elevation={10}
            style={{
              display: "flex",
              flexDirection: "column",
              alignItems: "center",
              justifyContent: "center",
              height: 75,
              width: 75,
            }}
          >
            <Typography variant="h5">ON</Typography>
            <Button onClick={f1} variant="contained" color="secondary">
              {val1}
            </Button>
          </StyledPaper>
        </Grid>
        <Grid item>
          <StyledPaper
            elevation={10}
            style={{
              display: "flex",
              flexDirection: "column",
              alignItems: "center",
              justifyContent: "center",
              height: 75,
              width: 75,
            }}
          >
            <Typography variant="h5" color="white">
              AB
            </Typography>
            <Button onClick={f2} variant="contained" color="secondary">
              {val2}
            </Button>
          </StyledPaper>
        </Grid>
        <Grid item>
          <StyledPaper
            elevation={10}
            style={{
              display: "flex",
              flexDirection: "column",
              alignItems: "center",
              justifyContent: "center",
              height: 75,
              width: 75,
            }}
          >
            <Typography variant="h5" color="white">
              BC
            </Typography>
            <Button onClick={f3} variant="contained" color="secondary">
              {val3}
            </Button>
          </StyledPaper>
        </Grid>
      </Grid>
    </ThemeProvider>
  );
}

export default App;
