    // Solve the Momentum equation

    tmp<fvVectorMatrix> tUrelEqn
    (
        fvm::div(phi, Urel)
      + turbulence->divDevRhoReff(Urel)
      + rho*SRF->Su()
      ==
        fvOptions(rho, Urel)
    );
    fvVectorMatrix& UrelEqn = tUrelEqn.ref();

    UrelEqn.relax();

    fvOptions.constrain(UrelEqn);

    solve(UrelEqn == -rho*fvc::grad(p));

    fvOptions.correct(Urel);
