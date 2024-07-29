import { useState } from "react";
import './App.css';
import { DisplayData } from "./DisplayData";
import { GenericSort } from "./GenericSort";
import { coursesList, studentList } from './data';
import { AppName, Level, Student } from "./interfaces";


function App({ name }: AppName) {
  const [student, setStudent] = useState<Student | null>(null);
  const [list, setList] = useState(studentList)

  const addStudent = () => {
    setStudent({
      name: "Anatony",
      surname: "Tunnemari",
      age: 20,
      level: Level.Undergraduate
    })
  }

  const sortData = () => {
    GenericSort(studentList, "age")
    setList([...studentList])
  }

  return (
    <div className="App">
      <h1>{name}</h1>
      <p><b>{student?.name} {student?.surname}</b></p>
      <button onClick={addStudent}>Add Student</button>
      <br />
      <button onClick={sortData}>Sort Data</button>
      <h3>List of Students</h3>
      <div>
        <DisplayData items={studentList} />
      </div>
      <h3>List of Courses</h3>
      <div>
        <DisplayData items={coursesList} />
      </div>
    </div>
  )
}

export default App;