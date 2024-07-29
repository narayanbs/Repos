import { renderHook, act } from '@testing-library/react-hooks';

import { useForm } from './useForm';

describe('useForm', () => {
  describe('smoke tests', () => {
    it('should be a function', () => {
      expect(typeof useForm).toBe('function');
    });

    it('should require the `validations` option', () => {
      renderHook(() => {
        expect(() => {
          useForm({});
        }).toThrow('the option `validations` is required');
      });
    });

    it('should require the validation option to be an object', () => {
      renderHook(() => {
        expect(() => {
          useForm({
            validations: true,
          });
        }).toThrow('the option `validations` should be an object');
      });
    });
  });

  describe('validateField', () => {
    describe('required', () => {
      it("should return a default error message for fields that don't have a value", () => {
        const { result } = renderHook(() => useForm({
          validations: {
            name: {
              required: true,
            },
          },
        }));

        expect(result.current.validateField('name', '')).toBe('required');
      });

      it('should return a custom error message', () => {
        const { result } = renderHook(() => useForm({
          validations: {
            name: {
              required: 'the field "name" is required',
            },
          },
        }));

        expect(result.current.validateField('name', '')).toBe('the field "name" is required');
      });
    });

    describe('pattern', () => {
      it('should return an error message if the value does not satisfy the pattern', () => {
        const { result } = renderHook(() => useForm({
          validations: {
            email: {
              pattern: {
                value: /\w+@\w+\.com/gi,
              },
            },
          },
        }));

        expect(result.current.validateField('email', '')).toBe('invalid');
      });

      it('should return an custom error message if the message attribute exists', () => {
        const { result } = renderHook(() => useForm({
          validations: {
            email: {
              pattern: {
                value: /\w+@\w+\.com/gi,
                message: 'Invalid e-mail',
              },
            },
          },
        }));

        expect(result.current.validateField('email', '')).toBe('Invalid e-mail');
      });
    });

    describe('validate', () => {
      let validateMock;
      let hook;

      beforeEach(() => {
        validateMock = jest.fn((value) => {
          if (Number(value) < 18) {
            return 'You are not able to get a drive permission';
          }

          return '';
        });

        const { result } = renderHook(() => useForm({
          validations: {
            age: {
              validate: validateMock,
            },
          },
        }));

        hook = result.current;
      });

      it('should execute the validate function passing the field value', () => {
        hook.validateField('age', '10');

        expect(validateMock).toHaveBeenCalledWith('10');
      });

      it('should be executed and return a string', () => {
        hook.validateField('age', '10');

        expect(validateMock).toHaveBeenCalled();
        expect(typeof validateMock.mock.results[0].value).toBe('string');
      });

      it('should return an error message', () => {
        hook.validateField('age', '10');

        expect(validateMock.mock.results[0].value).toBe('You are not able to get a drive permission');
      });

      it('should return an empty string when value is valid', () => {
        hook.validateField('age', '20');

        expect(validateMock.mock.results[0].value).toBe('');
      });
    });
  });

  describe('bindField', () => {
    it('should validate the name parameter', () => {
      const { result } = renderHook(() => useForm({
        validations: {
          name: {
            required: true,
          }
        }
      }));

      expect(() => {
        result.current.bindField();
      }).toThrow('The field name parameter is required');

      expect(() => {
        result.current.bindField(1);
      }).toThrow('The field name should be a string');
    });

    it('should return an object with value and onChange attributes', () => {
      const { result } = renderHook(() => useForm({
        validations: {
          name: {
            required: true,
          }
        }
      }));

      expect(result.current.bindField('name')).toEqual({
        value: expect.any(String),
        onChange: expect.any(Function),
      });
    });

    describe('onChange', () => {
      it('should update the Hook state when called', () => {
        const { result } = renderHook(() => useForm({
          validations: {
            name: {
              required: true,
            },
          },
        }));

        const bindFieldResult = result.current.bindField('name');

        act(() => {
          bindFieldResult.onChange({ target: { value: 'John' } });
        });

        expect(result.current.values.name).toBe('John');
        expect(result.current.errors.name).toBe('');

        act(() => {
          bindFieldResult.onChange({ target: { value: '' } });
        });

        expect(result.current.values.name).toBe('');
        expect(result.current.errors.name).toBe('required');
      });
    });
  });

  describe('initialValues', () => {
    it('should trhow an Error if the initialValues is not an object', () => {
      renderHook(() => {
        expect(() => {
          useForm({
            initialValues: true,
          })
        }).toThrow('the option `initialValues` should be an object');
      });
    });

    it('should initialize the values state with the initial values', () => {
      const { result } = renderHook(() => useForm({
        initialValues: {
          name: 'Carlos',
        },
        validations: {},
      }));

      expect(result.current.values.name).toBe('Carlos');
    });
  });

  describe('isValid', () => {
    it('should be a function', () => {
      const { result } = renderHook(() => useForm({
        validations: {},
      }));

      expect(typeof result.current.isValid).toBe('function');
    });

    it('should return false when it finds any error on the form', () => {
      const { result } = renderHook(() => useForm({
        initialValues: {
          name: 'Carlos',
          surname: '',
        },
        validations: {
          name: {
            required: true,
          },
          surname: {
            required: true,
          },
          birthDate: {
            pattern: {
              value: /^\d{2}\/\d{2}\/\d{4}$/gi,
              message: 'invalid date',
            },
          },
        },
      }));

      expect(result.current.isValid()).toBe(false);
    });

    it('should return true if all the form fields are valid', () => {
      const { result } = renderHook(() => useForm({
        initialValues: {
          name: 'Carlos',
          surname: 'Silva',
          birthDate: '28/10/1990',
        },
        validations: {
          name: {
            required: true,
          },
          surname: {
            required: true,
          },
          birthDate: {
            pattern: {
              value: /^\d{2}\/\d{2}\/\d{4}$/gi,
              message: 'invalid date',
            },
          },
        },
      }));

      expect(result.current.isValid()).toBe(true);
    });
  });
});
