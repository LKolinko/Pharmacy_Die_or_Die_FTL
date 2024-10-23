import { List, ListItem, ListSubheader, ListItemText, Stack } from "@mui/material"
import {SxProps, useTheme } from "@mui/material"
import {useEffect, useState} from "react"

const useStyles = () => {
    const theme = useTheme();
    const palette = theme.palette;
    return {
        list: {
            backgroundColor: palette.surface.main,
            width: "100%",
            height: "100%",
            display: "flex",
            flexDirection: "column",
            borderRadius: "0px",
            PaddingBottom: "0px"
        },
        cell: {
            backgroundColor: palette.secondaryContainer.main,
            borderRadius: "2px",
            PendingTop: "0px",
            PaddingBottom: "1px",
            margin: "0px",
        },
        header: {
            backgroundColor: palette.secondaryContainer.main,
            borderRadius: "2px",
            PendingTop: "0px",
            PaddingBottom: "1px",
            margin: "1px",
        },
        stack: {
            PendingTop: "1px",
            PaddingBottom: "1px",
            margin: "1px",
        },
        celltext: {
            alignItems: "center",
        }
    }
}

interface DayStatistic {
    number: Number,
    solve: Number,
    profit: Number,
    cnt_req: Number
}

const requestGetAll = async () => {
    try {
        const response = await (await fetch('http://0.0.0.0:8080/GetDaysStatistic', {
            method: 'GET',
            headers: {
                'Content-Type': 'application/json'
            }
        })).json();
        return response as DayStatistic[];
    } catch (e) {
        console.log(e);
    }
}

const DaysStatistic = () => {
    const [Items, setItems] = useState<DayStatistic[]>([]);
    useEffect(() => {
        const GetData = async () => {
            const answer = await requestGetAll();
            if (answer != undefined) {
                setItems(answer);
            }
        }
        GetData()
    }, []);
    return (
        <List sx={useStyles().list}>
            <li key={`section-${"names"}`}>
                <ul>
                    <Stack spacing={'1px'} direction="row" sx={useStyles().stack}>
                        <ListItem sx={useStyles().cell}>
                            <ListItemText primary={"Number"} />
                        </ListItem>
                        <ListItem sx={useStyles().cell}>
                            <ListItemText primary={"Solved"} />
                        </ListItem>
                        <ListItem sx={useStyles().cell}>
                            <ListItemText primary={"Profit"} />
                        </ListItem>
                        <ListItem sx={useStyles().cell}>
                            <ListItemText primary={"Requests"} />
                        </ListItem>
                    </Stack>
                </ul>
            </li>
            {Items.map((day) => (
                <li key={`section-${day.number}`}>
                    <ul>
                        <Stack spacing={'1px'} direction="row" sx={useStyles().stack}>
                            <ListItem sx={useStyles().cell}>
                                <ListItemText primary={`${day.number}`} />
                            </ListItem>
                            <ListItem sx={useStyles().cell}>
                                <ListItemText primary={`${day.solve}`} />
                            </ListItem>
                            <ListItem sx={useStyles().cell}>
                                <ListItemText primary={`${day.profit}`} />
                            </ListItem>
                            <ListItem sx={useStyles().cell}>
                                <ListItemText primary={`${day.cnt_req}`} />
                            </ListItem>
                        </Stack>
                    </ul>
                </li>
            ))}
        </List>
    );
}
export default DaysStatistic;