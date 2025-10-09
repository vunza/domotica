import '../css/styles.scss';
import img from '../assets/orquidea.jpeg';
import { soma, subtracao, multiplicacao, divisao } from './calculos.js';

// Crear un elemento HTML/h1 
const textComponent = () => {
    const elH1 = document.createElement('h1');
    elH1.innerHTML = 'Hello Webpack';
    // Agregar una clase creada en styles.scss
    elH1.classList.add('title');
    elH1.style.textDecoration = 'underline';
    return elH1;
};

const imageComponent = () => {
    const imgEl = new Image(150, 150);
    imgEl.src = img;
    return imgEl;
};

console.log('Soma: ', soma(2, 3));
console.log('Subtração: ', subtracao(5, 2));
console.log('Multiplicação: ', multiplicacao(3, 4));
console.log('Divisão: ', divisao(20, 2));

// Agregar los elementos al body
document.body.appendChild(textComponent());
document.body.appendChild(imageComponent());
