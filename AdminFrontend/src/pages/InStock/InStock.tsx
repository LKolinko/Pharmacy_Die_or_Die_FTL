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

/*
int64_t dosage_, quantity_, expiration_date_;
    std::string name_, type_, group_;
    int64_t wholesale_prices_, retail_price_;
*/

interface Drug {
    name: string
    dosage: string
    quantity: string
    expiration_date: string
    wholesale_prices: string
    retail_price: string
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
    const a: DrugTypeGroup = { name: "таблетка", data: [ {name: "Нурафен", dosage: "1.5", quantity: "100", expiration_date: "09.01.2024", wholesale_prices: "100", retail_price: "90" } ] };
    const Items: DrugGroup[] = [{ name: "Grup1", list: [ a ] }, { name: "Grup2", list: [ a ] }]

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
                                    <ListItem sx={useStyles().cell}>
                                        <ListItemText primary={"Expiration date"} />
                                    </ListItem>
                                    <ListItem sx={useStyles().cell}>
                                        <ListItemText primary={"Wholesale prices"} />
                                    </ListItem>
                                    <ListItem sx={useStyles().cell}>
                                        <ListItemText primary={"Retail prices"} />
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
                                    <ListItem key={`item-${drug.expiration_date}-${drug.expiration_date}`} sx={useStyles().cell}>
                                        <ListItemText primary={`${drug.expiration_date}`} />
                                    </ListItem>
                                    <ListItem key={`item-${drug.wholesale_prices}-${drug.wholesale_prices}`} sx={useStyles().cell}>
                                        <ListItemText primary={`${drug.wholesale_prices}`} />
                                    </ListItem>
                                    <ListItem key={`item-${drug.retail_price}-${drug.retail_price}`} sx={useStyles().cell}>
                                        <ListItemText primary={`${drug.retail_price}`} />
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