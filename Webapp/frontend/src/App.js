import React, { useEffect, useRef } from "react";
import { useState } from "react";
import "./App.css";

import "fontsource-roboto";
import { Grid } from "@mui/material";
import { ThemeProvider, createTheme, styled } from "@mui/material/styles";
import Button from "@mui/material/Button";
import { Typography } from "@mui/material";

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
  const [val4, setVal4] = useState(0); // For UN, unsorted
  const ws = useRef(null);
  // Websocket code
  // let validProvinceList = ["ON", "AB", "BC", "UN"];
  useEffect(() => {
    ws.current = new WebSocket("ws://localhost:8000/data/ws");
    ws.current.onopen = () => console.log("websocket open");
    ws.current.onmessage = (e) => {
      try {
        let provinceName = e.data; // Province name
        console.log(provinceName);
        switch (provinceName) {
          case "ON":
            console.log("In ON");
            setVal1((x) => x + 1);
            break;
          case "AB":
            console.log("In AB");
            setVal2((x) => x + 1);
            break;
          case "BC":
            console.log("In BC");
            setVal3((x) => x + 1);

            break;
          case "UN":
            console.log("In UN");
            setVal4((x) => x + 1);
            break;
          default:
            throw new Error("Invalid province name");
        }
      } catch (e) {
        console.log(e);
        console.log("Error in parsing websocket");
      }
    };
  }, []);

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
          backgroundColor="blue"
          sx={{
            height: "10rem",
            width: "15rem",
            margin: "2rem",
            display: "flex",
            flexDirection: "column",
          }}
        >
          <Typography variant="h3" color="white" margin="10px">
            Unsorted
          </Typography>
          <Button
            className="Data__Button--ON"
            // onClick={f1}
            variant="contained"
            color="secondary"
          >
            {val4}
          </Button>
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
            <Typography variant="h3">ON</Typography>
            <Button
              className="Data__Button--ON"
              // onClick={f1}
              variant="contained"
              color="secondary"
            >
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
              height: "10rem",
              width: "10rem",
            }}
          >
            <Typography variant="h3" color="white">
              AB
            </Typography>
            <Button
              //onClick={f2}
              variant="contained"
              color="secondary"
            >
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
