import React from 'react';

import { useForm } from '../useForm';

const SimpleForm = () => {
  const handleSubmit = e => {
    e.preventDefault();

    console.log('values', values);
  };

  const { values, errors, bindField, isValid } = useForm({
    validations: {
      name: {
        pattern: {
          value: /^\w{3,50}$/,
          message: 'invalid name'
        }
      },
      birthDate: {
        pattern: {
          value: /^\d{2}\/\d{2}\/\d{4}$/,
          message: 'invalid date format (dd/mm/yyyy)'
        }
      }
    }
  });

  return (
    <form onSubmit={handleSubmit}>
      <div>
        <label htmlFor="name">name</label>
        <input type="text" name="name" id="name" {...bindField('name')} />
        {errors.name && <p>{errors.name}</p>}
      </div>

      <div>
        <label htmlFor="birthDate">birth date</label>
        <input
          type="text"
          name="birthDate"
          id="birthDate"
          {...bindField('birthDate')}
        />
        {errors.birthDate && <p>{errors.birthDate}</p>}
      </div>

      <button type="submit" disabled={!isValid()}>
        submit
      </button>
    </form>
  );
};

export default SimpleForm;
