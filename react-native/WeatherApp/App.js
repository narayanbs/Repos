
import { NavigationContainer } from "@react-navigation/native";
import React from "react";
import { ActivityIndicator, StyleSheet, View } from "react-native";
import ErrorItem from "./src/components/ErrorItem";
import Tabs from "./src/components/Tabs";
import { useGetWeather } from "./src/hooks/useGetWeather";



const App = () => {
  const [loading, error, weather] = useGetWeather()

  if (weather && weather.list && !loading) {
    return (
      <NavigationContainer>
        <Tabs weather={weather} />
      </NavigationContainer>
    )
  }
  return (
    <View style={styles.container}>
      {error ? (<ErrorItem />) : (<ActivityIndicator size={"large"} color={"blue"} />)}
    </View>
  )
}


const styles = StyleSheet.create({
  container: {
    justifyContent: "center",
    flex: 1
  }
})
export default App;