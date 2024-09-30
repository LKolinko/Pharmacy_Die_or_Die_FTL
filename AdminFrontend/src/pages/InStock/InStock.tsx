import { List, ListItem, ListSubheader, ListItemText, Stack } from "@mui/material"
import {SxProps, useTheme} from "@mui/material"

const useStyles = () => {
    const theme = useTheme();
    const palette = theme.palette;
    return {
        list: {
            backgroundColor: palette.surface.main,
            width: "100%",
            height: "500px",
            overflow: "scroll",
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

interface Drug {
    name: string
    dosage: Number
    quantity: Number
}

interface DrugTypeGroup {
    name: string
    data: Drug[]
}

interface DrugGroup {
    name: string
    list: DrugTypeGroup[]
}

const InStock = () => {
    const a: DrugTypeGroup = { name: "таблетка", data: [ {name: "Нурафен", dosage: 1.5, quantity: 100}, 
                                                        {name: "Ношпа", dosage: 0.5, quantity: 52} ] };
    const b: DrugTypeGroup = { name: "not таблетка", data: [ {name: "Нурафен", dosage: 1.5, quantity: 100}, 
                            {name: "Ношпа", dosage: 0.5, quantity: 52} ] };
    const Items: DrugGroup[] = [{ name: "Grup1", list: [ a, b ] }, { name: "Grup2", list: [ a, b ] }]

    return (
        <List sx={useStyles().list}>
        {Items.map((drug_group) => (
            <li key={`section-${drug_group.name}`}>
              <ul>
                <ListSubheader sx={useStyles().header} disableSticky>{`${drug_group.name}`}</ListSubheader>
                {drug_group.list.map((drug_type_group) => (
                    <li key={`section-${drug_type_group.name}`}>
                            <ListSubheader sx={useStyles().header} disableSticky>{`${drug_type_group.name}`}</ListSubheader>
                            <Stack spacing={'1px'} direction="row" sx={useStyles().stack}>
                                    <ListItem sx={useStyles().cell}>
                                        <ListItemText primary={"Name"} />
                                    </ListItem>
                                    <ListItem sx={useStyles().cell}>
                                        <ListItemText primary={"Dosage"} />
                                    </ListItem>
                                    <ListItem sx={useStyles().cell}>
                                        <ListItemText primary={"Quantity"} />
                                    </ListItem>
                                </Stack>
                            {drug_type_group.data.map((drug) => (
                                <Stack spacing={'1px'} direction="row" sx={useStyles().stack}>
                                    <ListItem key={`item-${drug.name}-${drug.name}`} sx={useStyles().cell}>
                                        <ListItemText primary={`${drug.name}`} />
                                    </ListItem>
                                    <ListItem key={`item-${drug.dosage}-${drug.dosage}`} sx={useStyles().cell}>
                                        <ListItemText primary={`${drug.dosage}`} />
                                    </ListItem>
                                    <ListItem key={`item-${drug.quantity}-${drug.quantity}`} sx={useStyles().cell}>
                                        <ListItemText primary={`${drug.quantity}`} />
                                    </ListItem>
                                </Stack>
                            ))}
                    </li>
                ))}
              </ul>
            </li>
          ))}
        </List>
    );
}
export default InStock;