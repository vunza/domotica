const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const loader = require('sass-loader');

module.exports = {
    entry: path.resolve(__dirname, 'src/js/principal.js'),
    mode: 'production'/*development|none*/,
    output: {
        //filename: 'principal.js',
        filename: '[name]-[contenthash].js',
        path: path.resolve(__dirname, 'dist'),
        clean: true
    },    
    target: 'web'/*node*/,
    module:{
        rules: [
            {
                test: /\.(sa|sc|c)ss$/,
                use: [
                    MiniCssExtractPlugin.loader,
                    'css-loader',
                    'sass-loader'
                ]
            },           
            {
                 test: /\.css$/i,
                 use: [
                     'style-loader',
                     'css-loader',                    
                 ]
             },
            {
                test: /\.js$/,
                exclude: /node_modules/,
                use: {
                    loader: 'babel-loader',
                    options: {
                        presets: ['@babel/preset-env']
                    }
                }
            },
            {
                test: /\.(png|svg|jpg|jpeg|gif|bmp)$/i,
                loader: 'file-loader',
                options: {
                    name: '[name].[ext]'
                }
            }
        ]
    },
    plugins: [
        // Empaquetar varios archivos HTML
        new HtmlWebpackPlugin({
            filename: 'index.html',
            template: path.resolve(__dirname, 'src/public/index.html'),             
            minify: true // Optimizar HTML        
        }),
         new HtmlWebpackPlugin({
            filename: 'auxiliar.html',
            template: path.resolve(__dirname, 'src/public/auxiliar.html'),              
            minify: true // Optimizar HTML       
        }),
        // Empaquetar CSS en un archivo separado
        new MiniCssExtractPlugin({ 
            //filename: 'styles.css',
            filename: '[name].[contenthash].css'
        })
    ]
};



