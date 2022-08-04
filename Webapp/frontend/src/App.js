import React, { useEffect, useRef } from "react";
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

import ProvinceContainer from "./components/ProvinceContainer";
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

// Make responsive text size
// const { breakpoints, typography: { pxToRem } } = defaultTheme
let theme = {
  ...defaultTheme,
  typography: {
    h1: {
      fontSize: "5rem",
      "@media (min-width:600px)": {
        fontSize: "5rem",
      },
      [defaultTheme.breakpoints.up("sm")]: {
        fontSize: "3rem",
      },
    },
  },
};

function App() {
  const [val1, setVal1] = useState(0); // For ON
  const [val2, setVal2] = useState(0); // For AB
  const [val3, setVal3] = useState(0); // For BC

  const ws = useRef(null);
  // Websocket code
  let validProvinceList = ["ON", "AB", "BC"];
  useEffect(() => {
    ws.current = new WebSocket("ws://localhost:8000/data/ws");
    ws.current.onopen = () => console.log("websocket open");
    ws.current.onmessage = (e) => {
      let data;
      try {
        let provinceName = e.data; // Province name
        console.log(provinceName);
        switch (provinceName) {
          case "ON":
            setVal1((x) => x + 1);
            break;
          case "AB":
            setVal2((x) => x + 1);
            break;
          case "BC":
            console.log(val3);
            setVal3((x) => x + 1);
            break;
          default:
            throw "Invalid province name";
        }
      } catch (e) {
        console.log(e);
        console.log("Error in parsing websocket");
        throw "Error in parsing websocket";
      }
    };
  }, []);

  function f1(e) {
    setVal1(val1 + 1);
  }
  function f2() {
    setVal2(val2 + 1);
  }
  // function f3() {
  //   val3.current + 1;
  // }
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
            height: "10rem",
            width: "15rem",
            margin: "2rem",
          }}
        >
          <Typography variant="h3" color="primary">
            Start Sorting
          </Typography>
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
      <Grid
        className="Data"
        container
        padding="25px"
        justifyContent="space-around"
      >
        <ProvinceContainer name="ON" count={val1} theme={theme} />

        <Grid item>
          <StyledPaper
            elevation={10}
            style={{
              display: "flex",
              flexDirection: "column",
              alignItems: "center",
              justifyContent: "center",
              height: "10rem",
              width: "10rem",
            }}
          >
            <Typography variant="h3" color="white">
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
              height: "10rem",
              width: "10rem",
            }}
          >
            <Typography variant="h3" color="white">
              BC
            </Typography>
            <Button variant="contained" color="secondary">
              {val3}
            </Button>
          </StyledPaper>
        </Grid>
      </Grid>
    </ThemeProvider>
  );
}

export default App;
