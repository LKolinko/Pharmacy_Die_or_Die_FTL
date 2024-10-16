import { routerType } from "../types/router.types";
import ColorSystem from "./ColorSystem/ColorSystem";
import AddItem from "./AddItem/AddItem";
import InStock from "./InStock/InStock";
import Generation from "./Generation/Generation";
import InProgress from "./InProgress/InProgress";
import SolveToday from "./SolveToday/SolveToday";
import TotalSolve from "./TotalSolve/TotalSolve";
import DaysStatistic from "./DaysStatistic/DaysStatistic";

const pagesData: routerType[] = [
    {
        path: "ColorSystem",
        element: <ColorSystem />,
        title: "Color System"
    },
    {
        path: "AddItem",
        element: <AddItem />,
        title: "Add Item"
    },
    {
        path: "InStock",
        element: <InStock />,
        title: "In Stock"
    },
    {
        path: "Generation",
        element: <Generation />,
        title: "Generation"
    },
    {
        path: "SolveToday",
        element: <SolveToday />,
        title: "SolveToday"
    },
    {
        path: "InProgress",
        element: <InProgress />,
        title: "InProgress"
    },
    {
        path: "TotalSolve",
        element: <TotalSolve />,
        title: "TotalSolve"
    },
    {
        path: "DaysStatistic",
        element: <DaysStatistic />,
        title: "DaysStatistic"
    },
];

export default pagesData;