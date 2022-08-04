import { ThemeProvider, createTheme, styled } from "@mui/material/styles";
function StyledPaper(props) {
    
    const styledPaper = styled("Paper")(({ props.theme }) => ({
    backgroundColor: theme.palette.primary.main,
    color: theme.palette.primary.contrastText,
    }));
  return (
    <styledPaper
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
    <styledPaper/>
  );
}
