{
    // Calculate density from pressure
    rho.storePrevIter();
    rho = thermo.rho()();

}
