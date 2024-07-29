import { useState } from 'react';
import './App.css';
import { DisplayData } from './DisplayData';
import { genericSort } from './GenericSort';
import { coursesList, studentList } from './data';

function App({ name }) {
  const [student, setStudent] = useState(null);
  const [list, setList] = useState(studentList);

  const addStudent = () => {
    setStudent({
      name: "Antony",
      surname: "saint",
      age: 20,
      level: "Undergraduate"
    });
  }

  const sortData = () => {
    genericSort(studentList, "age");
    setList([...studentList]);
  }

  return (
    <div className="App">
      <h1>{name}</h1>
      <p>
        <b>{student?.name} {student?.surname}</b>
      </p>
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
  );
}

export default App;
