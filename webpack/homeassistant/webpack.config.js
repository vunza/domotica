const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const loader = require('sass-loader');

module.exports = {
    entry: path.resolve(__dirname, 'src/js/index.js'),
    mode: 'development'/*production|none*/,
    output: {
        filename: '[name]-[contenthash].js',
        path: path.resolve(__dirname, 'dist'),
        clean: true,
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
            // {
            //     exclude: /node_modules/,
            //     test: /\.css$/i,
            //     use:[
            //         {loader: 'style-loader'},
            //         {
            //             loader: 'css-loader',
            //             options: {
            //                 modules: true
            //             }
            //         }
            //     ]
            // },
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
        }),
         new HtmlWebpackPlugin({
            filename: 'login.html',
            template: path.resolve(__dirname, 'src/public/login.html'),         
        }),
        // Empaquetar CSS en un archivo separado
        new MiniCssExtractPlugin({ 
            filename: 'styles.css',
        })
    ]
};



