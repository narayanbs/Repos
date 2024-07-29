import React, { useEffect, useState } from "react";
import { Button, StyleSheet, Text, View } from "react-native";


const Counter = () => {
    let [count, setCount] = useState(0);
    let [newCount, setNewCount] = useState(0)
    useEffect(() => {
        console.log(`Count changed`)
    }, [count])
    return (
        <View style={styles.container}>
            <Text style={styles.title}>{`count: ${count}`}</Text>
            <Button color={"red"} title={"Increase the count"} onPress={() => { setCount(count + 1) }}></Button>
            <Button color={"green"} title={"Decrease the count"} onPress={() => { setCount(count - 1) }}></Button>
        </View>
    )
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        backgroundColor: "orange"
    },
    title: {
        alignSelf: "center",
        fontSize: 25,
        marginTop: 25
    }
})
export default Counter;