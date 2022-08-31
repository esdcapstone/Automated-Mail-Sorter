import { Grid } from "@mui/material";
import React, { useEffect, useRef } from "react";
import Button from "@mui/material/Button";
import { Typography } from "@mui/material";
import { Box } from "@mui/system";
import { Paper } from "@mui/material";

function ProvinceContainer(props) {
  return (
    <Grid item>
      <Typography variant="h3">{props.provinceName}</Typography>
      <Button
        className="Data__Button--ON"
        variant="contained"
        color="secondary"
      >
        {props.count}
      </Button>
    </Grid>
  );
}

export default ProvinceContainer;
